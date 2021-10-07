#include "main.h"

void deck::addcombat(variant owner) {
	for(auto& e : bsdata<combatcardi>()) {
		if(e.owner != owner)
			continue;
		for(auto i = 0; i < e.count; i++)
			add(&e);
	}
}

void deck::addcombat8(const char* abilities) {
	char temp[260]; stringbuilder sb(temp);
	for(auto i = 0; i < 8; i++) {
		sb.clear(); sb.add("%1Card%2i", abilities, i+1);
		variant v((const char*)temp);
		add(&v);
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
