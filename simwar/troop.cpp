#include "main.h"

void troop::clear() {
	memset(this, 0, sizeof(*this));
}

int troop::getbonus(variant id, const variants& source) {
	auto r = 0;
	for(auto v : source) {
		bonusi* pb = v;
		if(pb && pb->type == id)
			r += pb->bonus;
	}
	return r;
}

int troop::get(variant v) const {
	return type->get(v);
}

int troop::get(variant v, stringbuilder* sb) const {
	auto result = type->get(v);
	if(sb) {
		if(result > 0)
			sb->addn("[+%+1i]\t%2", result, getnm(type->id));
		else if(result < 0)
			sb->addn("[-%1i]\t%2", result, getnm(type->id));
	}
	return result;
}

void troop::kill() {
	clear();
}