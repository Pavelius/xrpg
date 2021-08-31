#include "code_pack.h"
#include "io_stream.h"

using namespace code;

BSDATAD(pack)

const char* urls::project;
const char* urls::projects = "projects";
const char* urls::library = "library";

namespace code {
const unsigned ValuseMask = 0x3FFFFFFF;
enum class base : unsigned {
	Literals, Symbols, ASTs,
};
struct ast {
	operation			type;
	pckh				right;
	pckh				left;
	constexpr bool isliteral() const { return type == operation::Literal; }
	constexpr bool issymbol() const { return type == operation::Symbol; }
	constexpr bool isnumber() const { return type == operation::Number; }
};
struct symbol {
	pckh				id = None; // String identificator
	pckh				parent = None; // Parent symbol
	pckh				result = None; // Result symbol type of expression
	unsigned			index = 0; // Position in source file
	unsigned			flags = 0;
	unsigned			local = 0;
	pckh				ast = None; // Abstract syntaxis tree index
	constexpr bool		is(symf v) const { return (flags & (1 << static_cast<int>(v))) != 0; }
	constexpr void		set(symf v) { flags |= 1 << static_cast<int>(v); }
};
}

static bool isnostrictorder(operation id) {
	switch(id) {
	case operation::Add: case operation::Mul:
	case operation::Or: case operation::And: case operation::Xor:
		return true;
	default:
		return false;
	}
}

static base getbase(pckh v) {
	return base(v >> 30);
}

static unsigned getvalue(pckh v) {
	return v & ValuseMask;
}

static unsigned getbasevalue(base b, pckh v) {
	return (static_cast<int>(b) << 30) | v;
}

pckh pack::find(const char* v, unsigned len) const {
	if(!v || !len)
		return None;
	auto p = strings.begin();
	auto pe = strings.end();
	auto s = *v;
	auto ns = len - 1;
	while(p < pe) {
		p = (char*)memchr(p, s, pe - p);
		if(!p)
			break;
		unsigned n1 = pe - p - 1;
		if(n1 < len)
			return -1;
		unsigned i = 1;
		for(; i < len; i++)
			if(p[i] != v[i])
				break;
		if(i==len && p[i]==0)
			return p - strings.begin();
		p++;
	}
	return None;
}

pckh pack::find(const char* v) const {
	if(!v)
		return None;
	return find(v, zlen(v));
}

pckh pack::add(const char* v, unsigned len) {
	if(!v || !len)
		return None;
	auto h = find(v, len);
	if(h != None)
		return h;
	auto result = strings.getcount();
	strings.reserve(result + len + 1);
	auto p = (char*)strings.ptr(result);
	memcpy(p, v, len + 1);
	p[len] = 0;
	strings.setcount(result + len + 1);
	return result;
}

pckh pack::add(const char* v) {
	if(!v)
		return None;
	return add(v, zlen(v));
}

pack* pack::addmodule(const char* url) {
	auto p = findmodule(url);
	if(p)
		return p;
	p = bsdata<pack>::add();
	memset(p, 0, sizeof(*p));
	p->create(url);
	return p;
}

pack* pack::findmodule(const char* url) {
	if(!url)
		return 0;
	for(auto& e : bsdata<pack>()) {
		if(!e.strings)
			continue;
		if(strcmp((char*)e.strings.data, url) == 0)
			return &e;
	}
	return 0;
}

pckh pack::find(operation type, pckh left, pckh right) const {
	for(auto& e : asts.records<ast>()) {
		if(e.type == type && e.left == left && e.right == right)
			return &e - (ast*)asts.begin();
	}
	return None;
}

pack::record pack::getsymbols() const {
	return record(getbasevalue(base::Symbols, 0), getbasevalue(base::Symbols, symbols.getcount()));
}

pckh pack::add(operation type, pckh left, pckh right) {
	auto i = find(type, left, right);
	if(i != None)
		return i;
	if(isnostrictorder(type)) {
		i = find(type, right, left);
		if(i != None)
			return i;
	}
	auto p = (ast*)asts.add();
	p->type = type;
	p->left = left;
	p->right = right;
	return p - (ast*)asts.begin();
}

pckh pack::addclass(pckh id, pckh result) {
	return addsym(id, Class, result, 0, 0);
}

void pack::addclasses(slice<string> source) {
	for(auto v : source) {
		auto id = add(v);
		addclass(id, id);
	}
}

void pack::create(const char* url) {
	clear();
	strings.setup(sizeof(char));
	symbols.setup(sizeof(symbol));
	asts.setup(sizeof(asts));
	addclass(This, add(url));
}

void pack::clear() {
	strings.clear();
	symbols.clear();
	asts.clear();
}

pckh pack::find(pckh id, pckh parent) const {
	for(auto& e : symbols.records<symbol>()) {
		if(e.id == id && e.parent == parent)
			return &e - (symbol*)symbols.begin();
	}
	return None;
}

pckh pack::addsym(pckh id, pckh parent, pckh result, unsigned index, unsigned flags) {
	auto v = find(id, parent);
	if(v == None) {
		auto p = (symbol*)symbols.add();
		p->id = id;
		p->parent = parent;
		p->result = result;
		p->index = index;
		p->flags = flags;
		p->ast = None;
		p->local = 0;
		v = p - (symbol*)symbols.begin();
	}
	return v;
}

const char* pack::getmodule(const char* url) {
	const char* collection[] = {szfurl(urls::projects), szfurl(urls::library)};
	for(auto p : collection) {
		auto n = zlen(p);
		if(!n)
			break;
		if(memcmp(url, p, n) == 0) {
			char temp[512]; stringbuilder sb(temp);
			sb.add(url + n + 1);
			auto pe = (char*)szext(temp);
			sb.change("/", ".");
			if(pe)
				pe[-1] = 0;
			return szdup(temp);
		}
	}
	return 0;
}

const char* pack::getname(pckh v0) const {
	auto v = getvalue(v0);
	switch(getbase(v0)) {
	case base::Literals:
		if(v < strings.getcount())
			return (const char*)strings.ptr(v);
		return "ErrorString";
	case base::Symbols:
		if(v >= symbols.getcount())
			return "ErrorSymbols";
		return getname(((symbol*)symbols.ptr(v))->id);
	default:
		return "";
	}
}

pckh pack::getresult(pckh vm) const {
	auto v = getvalue(vm);
	switch(getbase(vm)) {
	case base::Symbols:
		if(v >= symbols.getcount())
			return None;
		return ((symbol*)symbols.ptr(v))->result;
	default:
		return None;
	}
}

pckh pack::getparent(pckh vm) const {
	auto v = getvalue(vm);
	switch(getbase(vm)) {
	case base::Symbols:
		if(v >= symbols.getcount())
			return None;
		return ((symbol*)symbols.ptr(v))->parent;
	default:
		return None;
	}
}

unsigned pack::getflags(pckh vm) const {
	auto v = getvalue(vm);
	switch(getbase(vm)) {
	case base::Symbols:
		if(v >= symbols.getcount())
			return None;
		return ((symbol*)symbols.ptr(v))->flags;
	default:
		return None;
	}
}

pckh pack::reference(pckh sym) {
	return addsym(None, Pointer, sym, 0, 0);
}

static void serialx(io::stream& file, array& e, bool write_mode) {
	if(write_mode) {
		file.write(&e.count, sizeof(e.count));
		if(e.count)
			file.write(e.begin(), e.count * e.getsize());
	} else {
		e.clear();
		unsigned v = 0;
		file.read(&v, sizeof(v));
		if(v) {
			e.reserve(v);
			e.setcount(v);
			file.read(e.begin(), e.count * e.getsize());
		}
	}
}

static bool version(io::stream& file, const char* signature, int major, int minor, bool write_mode) {
	char temp[4];
	if(write_mode) {
		temp[0] = signature[0];
		temp[1] = signature[1];
		temp[2] = signature[2];
		temp[3] = 0;
		file.write(temp, sizeof(temp));
		temp[0] = '0' + major;
		temp[1] = '.';
		temp[2] = '0' + minor;
		file.write(temp, sizeof(temp));
	} else {
		file.read(temp, sizeof(temp));
		for(auto i = 0; i < 4; i++) {
			if(temp[i] != signature[i])
				return false;
		}
		file.read(temp, sizeof(temp));
		if(temp[0] - '0' != major)
			return false;
		if(temp[2] - '0' > minor)
			return false;
	}
	return true;
}

bool pack::serial(const char* url, bool write_mode) {
	io::file file(url, write_mode ? StreamWrite : StreamRead);
	if(!file)
		return false;
	version(file, "AST", 0, 1, write_mode);
	serialx(file, strings, write_mode);
	serialx(file, symbols, write_mode);
	serialx(file, asts, write_mode);
	return true;
}