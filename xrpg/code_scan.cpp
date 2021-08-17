#include "code.h"
#include "string.h"

using namespace code;

BSDATAD(memberi)
BSDATAD(typei)

struct corei {
	string				id, rule, url, comment;
	long long           number;
};

static const char*		p;
static rulea			this_rules;
static int				this_errors;
static const char*		this_url;
static corei			core;

void typei::clear() {
	memset(this, 0, sizeof(*this));
}

void memberi::clear() {
	memset(this, 0, sizeof(*this));
}

static typei* findtype(const char* id) {
	for(auto& e : bsdata<typei>()) {
		if(!e)
			continue;
		if(strcmp(e.id, id) == 0)
			return &e;
	}
	return 0;
}

static memberi* findmember(const char* id, const char* type) {
	for(auto& e : bsdata<memberi>()) {
		if(!e)
			continue;
		if(strcmp(e.id, id) == 0
			&& strcmp(e.type, type) == 0)
			return &e;
	}
	return 0;
}

static typei* addtype(const char* id) {
	auto p = findtype(id);
	if(!p)
		p = bsdata<typei>::addz();
	p->id = szdup(id);
	p->url = this_url;
	return p;
}

static memberi* addmember(const char* id, const char* type, const char* result) {
	auto p = findmember(id, type);
	if(!p)
		p = bsdata<memberi>::addz();
	p->id = szdup(id);
	p->url = this_url;
	p->type = szdup(type);
	p->result = szdup(result);
	return p;
}

static rulei* find_rule(const char* id) {
	for(auto& e : this_rules) {
		if(strcmp(e.name.id, id) == 0)
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

static void errors(error id, ...) {
	this_errors++;
}

static void number() {
    core.number = 0;
    if(!isnum(*p) || !(p[0]=='-' && isnum(p[1])))
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
}

static void add_member() {
}

static void test_type() {
	auto name = core.id.get();
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
	{"?global", {"%import", "%enum", "%global_declaration"}},
	{"import", {"import", "%url", "?%pseudoname", ";"}, add_type},
	{"pseudoname", {"as", "%id"}},
	{"url", {"%id", "?%.next_id"}, set_url},
	{"next_id", {"\\.", "%id"}},
	{"type", {"%id", "?.*"}, test_type},
	{"enum", {"enum", "{", ".%enum_values", "}", ";"}},
	{"enum_values", {"%enum_value", "?.%next_enum_value"}},
	{"next_enum_value", {",", "%enum_value"}},
	{"enum_value", {"%id", "?%enum_assign"}},
	{"enum_assign", {"=", "%constant"}},
	{"?constant", {"%expression"}, test_constant},
	{"static", {"static"}, apply_static},
	{"public", {"public"}, apply_public},
	{"?global_declaration", {"%member_variable", "%member_function"}},
	{"member", {"?%static", "?%public", "%type", "%id", "member_case"}},
	{"?member_case", {"%member_function", "%member_variable"}},
	{"member_function", {"(", "%declare_parameters", ")", "%block_statements"}},
	{"member_variable", {"%array_scope", "%initialization", ";"}},
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
		if(name.is(flag::Condition) && p1 != p) {
			// This token is match and only one in list must be valid
			break;
		}
		if(p1 == p) {
			// Case when token not work
			if(e.is(flag::Condition))
				// If tokens is optional continue executing
				continue;
			// This token is not match.
			if(pb != p)
				// Some of previous tokens match. This is error case
				errors(error::ExpectedP1, e.id);
            if(!name.is(flag::Condition))
                break;
		}
	}
	if(p != pb || !tokens[0])
		core.rule.set(pb, p - pb);
	if(special && (p != pb || !tokens[0]))
		special();
}

void tokeni::parse() const {
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

static const char* code_sample = "\
import core.crt;\n\
import core.collection.array as this;\n\
enum {OK, Cancel};\n\
";

void initialize_complex_grammar() {
	this_rules = c2_grammar;
	update_rules();
}

void code::parse(const char* url, const char* url_content, const lexer* pk) {
	this_url = url;
	p = code_sample;
	auto pr = find_rule("global");
	if(!pr)
		return;
	while(*p) {
		auto pb = p;
		pr->parse();
		if(pb == p) {
			errors(error::ExpectedP1, pr->name.id);
			break;
		}
	}
}
