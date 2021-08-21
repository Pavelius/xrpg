#include "code_pack.h"
#include "io_stream.h"

using namespace code;

BSDATAD(pack)

const char* urls::project;
const char* urls::projects = "projects";
const char* urls::library = "library";

symbol def_symbols[] = {
	{I32, Class, Class},
	{I16, Class, Class},
	{I8, Class, Class},
	{U32, Class, Class},
	{U16, Class, Class},
	{U8, Class, Class},
	{Void, Class, Class},
	{Bool, Class, Class},
	{None, Pointer, I8},
	{None, Pointer, Void},
	{True, This, Bool}, {False, This, Bool},
	{Pointer}, {Platform}, {Class},
	{SecStr}, {SecData}, {SecCode}, {SecBSS}, {SecLoc},
};
static_assert(sizeof(def_symbols) / sizeof(def_symbols[0]) == ((LastDef - FirstDef) + 1), "Invalid default symbols count");

const char* def_strings[] = {
	"i32", "i16", "i8",
	"u32", "u16", "u8",
	"void", "bool",
	"*i8", "*void",
	"true", "false",
	"*", "platform", "classes",
	"strings", "data", "code", "bss", "locale"
};
static_assert(sizeof(def_strings) / sizeof(def_strings[0]) == ((LastDef - FirstDef) + 1), "Invalid symbol strings count");

static bool isnostrictorder(operation id) {
	switch(id) {
	case operation::Add: case operation::Mul:
	case operation::Or: case operation::And: case operation::Xor:
		return true;
	default:
		return false;
	}
}

unsigned stringv::find(const char* v, unsigned len) const {
	if(v && len) {
		for(auto& e : def_strings) {
			if(strcmp(e, v) == 0)
				return (&e - def_strings) + FirstDef;
		}
		auto pe = end();
		auto s = *v;
		// Signature have first 4 bytes
		for(auto p = begin(); p < pe; p++) {
			if(*p != s)
				continue;
			unsigned n1 = pe - p - 1;
			if(n1 < len)
				return -1;
			if(memcmp(p + 1, v + 1, len) == 0)
				return p - begin();
		}
	}
	return None;
}

unsigned stringv::add(const char* v, unsigned len) {
	auto result = count;
	reserve(result + len + 1);
	memcpy(ptr(result), v, len + 1);
	setcount(result + len + 1);
	return result;
}

unsigned stringv::add(const char* v) {
	if(!v || v[0] == 0)
		return 0;
	auto c = zlen(v);
	auto i = find(v, c);
	if(i != None)
		return i;
	return add(v, c);
}

const char* stringv::get(pckh v) const {
	if(v >= FirstDef) {
		v -= FirstDef;
		if(v < sizeof(def_strings) / sizeof(def_strings[0]))
			return def_strings[v];
		return "";
	} else if(v < count)
		return (const char*)ptr(v);
	return "";
}

pack* pack::addmodule(const char* url) {
	auto p = findmodule(url);
	if(p)
		return p;
	p = bsdata<pack>::add();
	*p = pack();
	p->create(url);
	return p;
}

pack* pack::findmodule(const char* url) {
	for(auto& e : bsdata<pack>()) {
		if(!e.strings)
			continue;
		if(strcmp((char*)e.strings.data, url) == 0)
			return &e;
	}
	return 0;
}

pckh pack::find(operation type, pckh left, pckh right) const {
	for(auto& e : asts) {
		if(e.type == type && e.left == left && e.right == right)
			return &e - asts.begin();
	}
	return None;
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
	auto p = asts.add();
	p->type = type;
	p->left = left;
	p->right = right;
	return p - asts.begin();
}

pckh pack::addclass(pckh id, pckh result) {
	if(ispredefined(id))
		return id;
	return addsym(id, Class, result, 0, 0);
}

void pack::create(const char* url) {
	clear();
	addclass(This, add(url));
}

void pack::clear() {
	strings.clear();
	symbols.clear();
	asts.clear();
}

pckh pack::findsym(pckh id, pckh parent) const {
	for(auto& e : def_symbols) {
		if(e.id == id && e.parent == parent)
			return &e - def_symbols + FirstDef;
	}
	for(auto& e : symbols) {
		if(e.id == id && e.parent == parent)
			return &e - symbols.begin();
	}
	return None;
}

pckh pack::addsym(pckh id, pckh parent, pckh result, unsigned index, unsigned flags) {
	auto v = findsym(id, parent);
	if(v == None) {
		auto p = symbols.add();
		p->id = id;
		p->parent = parent;
		p->result = result;
		p->index = index;
		p->flags = flags;
		p->ast = None;
		v = p - symbols.begin();
	}
	return v;
}

void pack::setast(pckh sym, pckh ast) {
	auto p = getsymbol(sym);
	if(p)
		p->ast = ast;
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

unsigned pack::getresult(pckh sym) const {
	auto p = getsymbol(sym);
	if(!p)
		return None;
	if(p->parent == Class)
		return Class;
	return p->result;
}

pckh pack::getparent(pckh v) const {
	auto p = getsymbol(v);
	if(!p)
		return None;
	return p->parent;
}

unsigned pack::reference(pckh sym) {
	return addsym(None, Pointer, sym, 0, 0);
}

symbol* pack::getsymbol(pckh v) const {
	if(v < symbols.getcount())
		return const_cast<symbol*>(symbols.ptr(v));
	else if(v >= FirstDef && v <= (sizeof(def_strings) / sizeof(def_strings[0]) + FirstDef))
		return &def_symbols[v - FirstDef];
	return 0;
}

const ast* pack::getast(pckh v) const {
	if(v >= asts.getcount())
		return 0;
	return asts.ptr(v);
}

//pckh package::getleft(pckh v) const {
//	auto p = getast(v);
//	if(!p)
//		return None;
//	return p->left;
//}

bool pack::issymbol(pckh ast) const {
	auto p = getast(ast);
	if(!p)
		return false;
	return p->type == operation::Symbol;
}

bool pack::ispointer(pckh sym) const {
	auto p = getsymbol(sym);
	if(!p)
		return false;
	return p->parent == Pointer;
}

bool pack::ismethod(pckh sym) const {
	auto p = getsymbol(sym);
	if(!p)
		return false;
	return p->is(symf::Method);
}

void pack::addurl(stringbuilder& sb, const char* url, const char* id, const char* ext) {
	char temp[512]; stringbuilder s1(temp);
	s1.add(id); s1.change((char)'.', (char)'//');
	sb.add(url);
	sb.add(s1);
	sb.add(".");
	sb.add(ext);
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
	version(file, "PKG", 0, 1, write_mode);
	serialx(file, strings, write_mode);
	serialx(file, symbols, write_mode);
	serialx(file, asts, write_mode);
	return true;
}