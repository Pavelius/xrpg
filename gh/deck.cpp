#include "main.h"

void deck::addcombat(variant owner) {
	for(auto& e : bsdata<combatcardi>()) {
		if(e.owner != owner)
			continue;
		for(auto i = 0; i < e.count; i++)
			add(&e);
	}
}

void deck::shuffle() {
	zshuffle(data, count);
}

variant deck::get() {
	if(!count)
		return variant();
	auto r = data[0];
	remove(0);
	return r;
}

variant deck::look(int n) {
	if(n >= (int)count)
		return variant();
	return data[n];
}
