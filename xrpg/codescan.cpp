#include "crt.h"
#include "codescan.h"

int codescan::metrics::tabs = 4;

int codescan::getindex(const char* source, pointl pt) {
	auto y = 0, x = 0;
	auto p = source;
	auto i = -1;
	while(*p) {
		p = getnext(p, x, y);
		if(pt.y == y) {
			i = p - source;
			if(pt.x <= x)
				return p - source;
		} else if(pt.y < y)
			return i;
	}
	return -1;
}

void codescan::getstate(const char* source, int origin_y, int& cashe_y, pointl& size, int p1, pointl& pp1, int p2, pointl& pp2) {
	size.x = size.y = 0;
	auto y = 0, x = 0;
	auto p = source;
	cashe_y = y;
	while(*p) {
		switch(*p) {
		case 10: case 13:
			if(size.x < x)
				size.x = x;
			p = getnext(p, x, y);
			if(origin_y == y)
				cashe_y = p - source;
			break;
		case 9:
			p = getnext(p, x, y);
			break;
		default:
			x++; p++;
			break;
		}
		auto n = p - source;
		if(p1 == n) {
			pp1.x = x;
			pp1.y = y;
		}
		if(p2 == n) {
			pp2.x = x;
			pp2.y = y;
		}
	}
	if(size.x < x)
		size.x = x;
	size.y = y;
}

const char* codescan::getnext(const char* p, int& x, int& y) {
	switch(*p) {
	case 0:
		break;
	case 0x09:
		p++;
		x = ((x / metrics::tabs) + 1) * metrics::tabs;
		break;
	case 10:
		x = 0; y++;
		p++;
		if(*p == 13)
			p++;
		break;
	case 13:
		x = 0; y++;
		p++;
		if(*p == 10)
			p++;
		break;
	default:
		p++; x++;
		break;
	}
	return p;
}

const char* codescan::getnext(const char* p, int& x, int& y, group_s& type) {
	type = IllegalSymbol;
	if(*p == 0)
		type = WhiteSpace;
	else if(*p == 0x20 || *p == 9 || *p == 10 || *p == 13) {
		type = WhiteSpace;
		while(*p == 0x20 || *p == 9 || *p == 10 || *p == 13)
			p = getnext(p, x, y);
	} else if((*p == '-' && isnum(*p)) || isnum(*p)) {
		type = Number;
		if(*p == '-')
			p = getnext(p, x, y);
		if(p[0] == '0' && p[1] == 'x') {
			p = getnext(p, x, y);
			p = getnext(p, x, y);
			while(isnum(*p) || (*p >= 'A' && *p <= 'F') || (*p >= 'a' && *p <= 'f'))
				p = getnext(p, x, y);
		} else {
			while(isnum(*p))
				p = getnext(p, x, y);
		}
	} else if(*p == '\"') {
		type = String;
		p = getnext(p, x, y);
		while(*p && *p != '\"' && *p != 10 && *p != 13) {
			if(*p == '\\')
				p = getnext(p, x, y);
			p = getnext(p, x, y);
		}
		if(*p)
			p = getnext(p, x, y);
	} else if(p[0] == '/' && p[1] == '/') {
		type = Comment;
		p = getnext(p, x, y);
		p = getnext(p, x, y);
		while(*p) {
			if(*p == 10 || *p == 13) {
				p = getnext(p, x, y);
				break;
			} else
				p = getnext(p, x, y);
		}
	} else if(ischa(*p) || *p == '_') {
		type = Identifier;
		auto pb = p;
		while(ischa(*p) || *p == '_' || isnum(*p))
			p = getnext(p, x, y);
		//		auto pk = find(pb, p - pb);
		//		if(pk)
		//			type = pk->type;
	} else if(*p <= 64 || (*p >= 123 && *p <= 127)) {
		type = Operator;
		x++;
		return p + 1;
	} else
		return getnext(p, x, y);
	return p;
}