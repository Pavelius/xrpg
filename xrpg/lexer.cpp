#include "crt.h"
#include "lexer.h"

using namespace code;

int code::metrics::tabs = 4;

int code::getindex(const char* source, pointl pt) {
	pointl pos = {0, 0};
	auto p = source;
	auto i = -1;
	while(*p) {
		p = getnext(p, pos);
		if(pt.y == pos.y) {
			i = p - source;
			if(pt.x <= pos.x)
				return p - source;
		} else if(pt.y < pos.y)
			return i;
	}
	return p - source;
}

const char* code::getnext(const char* p, pointl& pos) {
	switch(*p) {
	case 0:
		break;
	case 0x09:
		p++;
		pos.x = ((pos.x / metrics::tabs) + 1) * metrics::tabs;
		break;
	case 10:
		pos.x = 0; pos.y++;
		p++;
		if(*p == 13)
			p++;
		break;
	case 13:
		pos.x = 0; pos.y++;
		p++;
		if(*p == 10)
			p++;
		break;
	default:
		p++; pos.x++;
		break;
	}
	return p;
}

const char* code::getnext(const char* p, pointl& pos, group_s& type, const lexer* pk) {
	type = IllegalSymbol;
	if(*p == 0)
		type = WhiteSpace;
	else if(*p == 0x20 || *p == 9 || *p == 10 || *p == 13) {
		type = WhiteSpace;
		while(*p == 0x20 || *p == 9 || *p == 10 || *p == 13)
			p = getnext(p, pos);
	} else if((*p == '-' && isnum(*p)) || isnum(*p)) {
		type = Number;
		if(*p == '-')
			p = getnext(p, pos);
		if(p[0] == '0' && p[1] == 'x') {
			p = getnext(p, pos);
			p = getnext(p, pos);
			while(isnum(*p) || (*p >= 'A' && *p <= 'F') || (*p >= 'a' && *p <= 'f'))
				p = getnext(p, pos);
		} else {
			while(isnum(*p))
				p = getnext(p, pos);
		}
	} else if(*p == '\"') {
		type = String;
		p = getnext(p, pos);
		while(*p && *p != '\"' && *p != 10 && *p != 13) {
			if(*p == '\\')
				p = getnext(p, pos);
			p = getnext(p, pos);
		}
		if(*p)
			p = getnext(p, pos);
	} else if(p[0] == '/' && p[1] == '/') {
		type = Comment;
		p = getnext(p, pos);
		p = getnext(p, pos);
		while(*p) {
			if(*p == 10 || *p == 13) {
				p = getnext(p, pos);
				break;
			} else
				p = getnext(p, pos);
		}
	} else if(ischa(*p) || *p == '_') {
		type = Identifier;
		auto pb = p;
		while(ischa(*p) || *p == '_' || isnum(*p))
			p = getnext(p, pos);
		if(pk) {
			auto pw = find(pk->keywords, {pb, p - pb});
			if(pw)
				type = Keyword;
		}
	} else if(*p == '{') {
		type = BlockBegin;
		pos.x++; p++;
	} else if(*p == '}') {
		type = BlockEnd;
		pos.x++; p++;
	} else if(*p == '[') {
		type = IndexBegin;
		pos.x++; p++;
	} else if(*p == ']') {
		type = IndexEnd;
		pos.x++; p++;
	} else if(*p == '(') {
		type = ExpressionBegin;
		pos.x++; p++;
	} else if(*p == ')') {
		type = ExpressionEnd;
		pos.x++; p++;
	} else if(*p <= 64 || (*p >= 123 && *p <= 127)) {
		type = Operator;
		auto pw = find(pk->operations, p);
		if(pw) {
			pos.x += pw->size;
			p += pw->size;
		} else {
			pos.x++;
			p++;
		}
		return p;
	} else
		return getnext(p, pos);
	return p;
}

const string* code::find(const worda& source, const string& v) {
	for(auto& e : source) {
		if(e.equal(v))
			return &e;
	}
	return 0;
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
			if(t.is(code::flag::Variable))
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
	updaterules();
	addkeywords(keywords);
}

void initialize_lexers() {
	for(auto& e : bsdata<lexer>()) {
		e.setgrammar();
		e.initialize();
	}
}