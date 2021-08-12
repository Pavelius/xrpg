#include "codescan.h"

bool codescan::isnextline(const char* ps, const char** pv) {
	if(*ps == 10) {
		ps++;
		if(*ps == 13)
			*ps++;
		if(pv)
			*pv = ps;
		return true;
	} else if(*ps == 13) {
		ps++;
		if(*ps == 10)
			*ps++;
		if(pv)
			*pv = ps;
		return true;
	}
	return false;
}

static int getnext(int x, char sym) {
	if(sym == '\t' && codescan::metrics::tabs)
		return ((x + (codescan::metrics::tabs - 1)) / codescan::metrics::tabs) * codescan::metrics::tabs;
	else
		return x + 1;
}

static int getpos(const char* source, const char* p1) {
	auto p = source;
	auto x = 0;
	int result = 0;
	while(*p && *p != 10 && *p != 13) {
		if(*p == '\t')
			x = getnext(x, *p);
		else
			x++;
		if(p1 == p)
			result = x;
		p++;
	}
	return result;
}

void codescan::getstate(const char* source, int origin_y, int& cashe_y, int& maximum_x, int& maximum_y) {
	maximum_x = maximum_y = 0;
	auto y = 0, x = 0;
	auto p = source;
	cashe_y = y;
	while(*p) {
		if(*p == 10 || *p == 13) {
			isnextline(p, &p);
			y++; x = 0;
			if(origin_y == y)
				cashe_y = p - source;
			if(maximum_x < x)
				maximum_x = x;
		} else {
			if(*p == '\t')
				x = getnext(x, *p);
			else
				x++;
			p++;
		}
		y++;
	}
	if(maximum_x < x)
		maximum_x = x;
	maximum_y = y;
}