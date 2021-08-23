#include "code.h"
#include "code_pack.h"
#include "string.h"

using namespace code;

const char*		code::p;
static rulea	this_rules;
pack*			code::this_pack;
corei			code::core;

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

void code::skipws() {
	p = skipspcr(p);
}

static void expected(const char* id) {
}

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
		if(core.success)
			core.success({pb, p - pb}, *this, e);
		if(e.is(flag::Stop))
			break;
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

void pack::parse(const char* url_content) {
	this_pack = this;
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

static const string* find(const array& source, string v) {
	for(auto& e : source.records<string>()) {
		if(e.equal(v))
			return &e;
	}
	return 0;
}

static void add(array& source, string v) {
	auto p = find(source, v);
	if(p)
		return;
	auto p1 = (string*)source.add();
	*p1 = v;
}

static void addkeywords(array& source) {
	for(auto& e : this_rules) {
		for(auto& t : e.tokens) {
			if(!t)
				break;
			if(t.is(flag::Variable))
				continue;
			if(ischa(t.id[0]))
				add(source, t.id);
		}
	}
}

void lexer::setgrammar() const {
	this_rules = grammar;
}

void lexer::initialize() {
	update_rules();
	addkeywords(keywords);
}

void initialize_lexers() {
	for(auto& e : bsdata<lexer>()) {
		e.setgrammar();
		e.initialize();
	}
}