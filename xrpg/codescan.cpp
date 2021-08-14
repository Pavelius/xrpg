#include "crt.h"
#include "codescan.h"

int codescan::metrics::tabs = 4;

int codescan::getindex(const char* source, pointl pt) {
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
	return -1;
}

void codescan::getstate(const char* source, int origin_y, int& cashe_y, pointl& size, int p1, pointl& pp1, int p2, pointl& pp2) {
	size.x = size.y = 0;
	pointl pos = {0, 0};
	auto p = source;
	cashe_y = pos.y;
	while(*p) {
		switch(*p) {
		case 10: case 13:
			if(size.x < pos.x)
				size.x = pos.x;
			p = getnext(p, pos);
			if(origin_y == pos.y)
				cashe_y = p - source;
			break;
		case 9:
			p = getnext(p, pos);
			break;
		default:
			pos.x++; p++;
			break;
		}
		auto n = p - source;
		if(p1 == n)
			pp1 = pos;
		if(p2 == n)
			pp2 = pos;
	}
	if(size.x < pos.x)
		size.x = pos.x;
	size.y = pos.y;
}

const char* codescan::getnext(const char* p, pointl& pos) {
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

const char* codescan::getnext(const char* p, pointl& pos, group_s& type, const lexer* pk) {
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
			auto pw = pk->find(pb, p - pb);
			if(pw)
				type = Keyword;
		}
	} else if(*p <= 64 || (*p >= 123 && *p <= 127)) {
		type = Operator;
		pos.x++;
		return p + 1;
	} else
		return getnext(p, pos);
	return p;
}

const codescan::lexer::word* codescan::lexer::find(const char* sym, unsigned size) const {
	for(auto& e : keywords) {
		if(e.size != size)
			continue;
		if(memcmp(e.id, sym, size) == 0)
			return &e;
	}
	return 0;
}