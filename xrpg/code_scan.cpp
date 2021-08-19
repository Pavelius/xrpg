#include "code.h"
#include "string.h"

using namespace code;

BSDATAD(memberi)
BSDATAD(typei)

struct corei {
	string				type, id, rule, url, comment;
	long long           number = 0;
};

static const char*		p;
static rulea			this_rules;
static int				this_errors;
static const char*		this_url;
static corei			core;

void typei::clear() {
	memset(this, 0, sizeof(*this));
}

typei* typei::find(const char* id, const char* url) {
	for(auto& e : bsdata<typei>()) {
		if(!e)
			continue;
		if(e.url && e.url != url)
			continue;
		if(strcmp(e.id, id) == 0)
			return &e;
	}
	return 0;
}

typei* typei::add(const char* id, const char* value, const char* url) {
	auto p = find(id, url);
	if(!p)
		p = bsdata<typei>::addz();
	p->id = szdup(id);
	if(!value)
		value = p->id;
	p->value = szdup(value);
	p->url = url; // can't be szdup(url) because szdup(0) return ""
	return p;
}

void memberi::clear() {
	memset(this, 0, sizeof(*this));
}

static rulei* find_rule(const char* id) {
	for(auto& e : this_rules) {
		if(strcmp(e.name, id) == 0)
			return &e;
	}
	return 0;
}

static void update_rules() {
	for(auto& r : this_rules) {
		for(auto& e : r.tokens) {
			if(!e)
				break;
			if(e.is(flag::Variable))
				e.rule = find_rule(e.id);
		}
	}
}

static bool ischab(char sym) {
	return (sym >= 'A' && sym <= 'Z')
		|| (sym >= 'a' && sym <= 'z')
		|| (sym == '_');
}

static void skipws() {
	p = skipspcr(p);
}

static void expected(const char* id) {
}

static char next_literal() {
	while(*p) {
		if(*p != '\\')
			return *p++;
		p++;
		switch(*p++) {
		case 0: return 0;
		case 'n': return '\n';
		case 't': return '\t';
		case 'r': return '\r';
		case '\\': return '\\';
		case '\'': return '\'';
		case '\"': return '\"';
		case '\n':
			if(p[0] == '\r')
				p++;
			break;
		case '\r':
			if(p[0] == '\n')
				p++;
			break;
		default: break;
		}
	}
	return 0;
}

static void literal() {
	if(*p != '\"')
		return;
	p++;
	while(*p && *p!='\"') {
		auto s = next_literal();
	}
	if(*p == '\"') {
		p++; skipws();
	}
}

static void number() {
	core.number = 0;
	if(!isnum(*p) && !(p[0] == '-' && isnum(p[1])))
		return;
	if(p[0] == '0') {
		if(p[1] == 'x') {
			p += 2;
			while(true) {
				char s = *p;
				if(s >= 'a' && s <= 'f')
					s = s - 'a' + 10;
				else if(s >= 'A' && s <= 'F')
					s = s - 'A' + 10;
				else if(s >= '0' && s <= '9')
					s = s - '0';
				else
					break;
				core.number = core.number * 16 + s;
				p++;
			}
		} else {
			while(*p >= '0' && *p <= '7') {
				core.number = core.number * 8 + *p - '0';
				p++;
			}
		}
	} else {
		while(*p >= '0' && *p <= '9') {
			core.number = core.number * 10 + *p - '0';
			p++;
		}
	}
	if(*p == 'f' || *p == 'e')
		p++;
	skipws();
}

static void identifier() {
	if(!ischab(*p))
		return;
	auto pb = p;
	while(ischa(*p))
		p++;
	core.id.set(pb, p - pb);
	skipws();
}

static void add_type() {
	typei::add(core.id.get(), core.url.get(), this_url);
}

static void add_member() {
}

static void test_type() {
	core.type = core.id;
	auto name = core.id.get();
	if(!typei::find(name, this_url))
		p = core.rule.begin();
}

static void test_constant() {
}

static void set_url() {
	core.url = core.rule;
}

static void apply_static() {
}

static void apply_public() {
}

static rulei c2_grammar[] = {
	{"id", {}, identifier},
	{"number", {}, number},
	{"string", {}, literal},
	{"global", {"?%import", "?%enum", "?%member_function", "?%member_variable"}},
	{"import", {"import", "%url", "?%pseudoname", ";"}, add_type},
	{"pseudoname", {"as", "%id"}},
	{"url", {"%id", ". ?%.id"}, set_url},
	{"type", {"%id", "?.*"}, test_type},
	{"enum", {"enum", "{", "%enum_value", ", ?.%enum_value", "}", ";"}},
	{"enum_value", {"%id", "?%enum_assign"}},
	{"enum_assign", {"=", "%constant"}},
	{"constant", {"%expression"}, test_constant},
	{"initialization", {"=", "%expression"}},
	{"static", {"static"}, apply_static},
	{"public", {"public"}, apply_public},
	{"member", {"%type", "%id"}},
	{"parameter", {"%member"}},
	{"parameters", {"%parameter", ", .?%parameter"}},
	{"global_flags", {"?%static", "?%public"}},
	{"member_function", {"?%global_flags", "%member", "(", "?%parameters", ")", "%block_statements"}},
	{"member_variable", {"?%global_flags", "%member", "?%array_scope", "?%initialization", ";"}},
	{"local_variable", {"?%static", "%member", "?%array_scope", "?%initialization"}},
	{"array_scope", {"[", "%expression", "]"}},
	{"expression", {"?%number", "?%string"}},
	{"block_statements", {"{", "?%single_statement", ".?%single_statement", "}"}},
	{"single_statement", {"?%statement", ";"}},
	{"statement", {"?%local_variable"}},
};

void rulei::parse() const {
	auto pb = p;
	for(auto& e : tokens) {
		if(!e)
			break;
		auto p1 = p;
		e.parse();
		if(e.is(flag::Repeat)) {
			auto p2 = p1;
			while(p2 != p) {
				p2 = p;
				e.parse();
			}
		}
		if(p1 == p) { // Case when token did not work
			if(e.is(flag::Condition)) // If tokens is optional continue executing
				continue;
			if(pb != p) {
				expected(e.id); // Some of previous tokens match. This is error case
				p = pb; // Rollback parser
			}
			break;
		}
	}
	if(special && (p != pb || !tokens[0])) {
		core.rule.set(pb, p - pb);
		special();
	}
}

void tokeni::parse() const {
	if(is(flag::ComaSeparated)) {
		if(p[0] == ',') {
			p++; skipws();
		}
	} else if(is(flag::PointSeparated)) {
		if(p[0] == '.') {
			p++; skipws();
		}
	}
	if(rule)
		// This is rule, not token
		rule->parse();
	else {
		// This is determinal, that must match input string
		auto n = zlen(id);
		if(memcmp(p, id, n) != 0)
			return;
		p += n;
		skipws();
	}
}

void initialize_complex_grammar() {
	this_rules = c2_grammar;
	update_rules();
}

void code::parse(const char* url, const char* url_content, const lexer* pk) {
	this_url = url;
	p = url_content;
	auto pr = find_rule("global");
	if(!pr)
		return;
	while(*p) {
		auto pb = p;
		pr->parse();
		if(pb == p) {
			expected(pr->name);
			break;
		}
	}
}
