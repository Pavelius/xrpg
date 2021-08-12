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

void codescan::getstate(const char* source, int origin_y, int& cashe_y, int& maximum_x, int& maximum_y, int& p1_x, int& p1_y, int& p2_x, int& p2_y, int p1, int p2) {
	p1_x = p1_y = p2_x = p2_y = -1;
	maximum_x = maximum_y = 0;
	cashe_y = -1;
	auto n = 0;
	auto p = source;
	while(*p) {
		auto i0 = p - source;
		if(origin_y == n)
			cashe_y = p - source;
		while(*p && *p!=10 && *p!=13)
			p++;
		auto i1 = p - source;
	}
}