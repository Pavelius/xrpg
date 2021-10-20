#include "main.h"

void troop::clear() {
	memset(this, 0, sizeof(*this));
}

int troop::getbonus(variant id, const variants& source) {
	auto r = 0;
	for(auto v : source) {
		bonusi* pb = v;
		if(pb && pb->type==id)
			r += pb->bonus;
	}
	return r;
}

int troop::get(variant v, stringbuilder* sb) const {
	auto r = 0;
	uniti* pu = type;
	if(pu) {
		r += pu->get(v);
		if(v==variant(Raid))
			r += get(Attack);
	}
	return r;
}

void troop::kill() {
	clear();
}