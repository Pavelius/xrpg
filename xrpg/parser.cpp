#include "parser.h"
#include "string.h"

using namespace code;

const char*			code::p;
rulea				code::this_rules;
corei				code::core;
errori				code::error;
static const char*	potent_p;
static const char*	potent_expected;

void corei::clear() {
	memset(this, 0, sizeof(*this));
}

void errori::clear() {
	memset(this, 0, sizeof(*this));
}

static void expected(const char* id) {
	potent_p = p;
	potent_expected = id;
}

static bool ischab(char sym) {
	return (sym >= 'A' && sym <= 'Z')
		|| (sym >= 'a' && sym <= 'z')
		|| (sym == '_');
}

static rulei* find_rule(const char* id) {
	for(auto& e : this_rules) {
		if(strcmp(e.name, id) == 0)
			return &e;
	}
	return 0;
}

void code::updaterules() {
	for(auto& r : this_rules) {
		for(auto& e : r.tokens) {
			if(!e)
				break;
			if(e.is(flag::Variable)) {
				e.rule = find_rule(e.id);
				if(!e.rule) {
					// TODO: fix error
				}
			}
		}
	}
}

static void comments() {
	if(p[0] == '/' && p[1] == '/') {
		p += 2;
		while(*p && *p != 10 && *p != 13)
			p++;
		p = skipspcr(p);
	}
}

void code::skipws() {
	while(*p) {
		p = skipspcr(p);
		auto p1 = p;
		comments();
		if(p1 != p)
			continue;
		break;
	}
}

void rulei::parse() const {
	auto pb = p;
	if(before)
		before();
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
		} else
			return;
	} else if(is(flag::PointSeparated)) {
		if(p[0] == '.') {
			p++; skipws();
		} else
			return;
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

void code::literal() {
	if(*p == '\"') {
		p++;
		while(*p && *p != '\"') {
			auto s = next_literal();
		}
		if(*p == '\"') {
			p++; skipws();
		}
	}
}

void code::number() {
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

void code::identifier() {
	if(!ischab(*p))
		return;
	auto pb = p;
	while(ischa(*p))
		p++;
	core.id.set(pb, p - pb);
	skipws();
}

bool code::parse(const char* url_content, const char* rule_name) {
	error.clear();
	p = url_content;
	if(!rule_name)
		rule_name = "global";
	auto pr = find_rule(rule_name);
	if(!pr) {
		error.expected = rule_name;
		return false;
	}
	skipws();
	while(*p) {
		auto pb = p;
		pr->parse();
		if(pb == p) {
			error.p = potent_p;
			error.expected = potent_expected;
			return false;
		}
	}
	return true;
}