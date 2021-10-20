#include "main.h"

void selector::match(landscape_s v, bool keep) {
	auto ps = begin();
	for(auto e : *this) {
		uniti* pu = e;
		if(!pu)
			continue;
		auto r = pu->encounter.is(v);
		if(r != keep)
			continue;
		*ps++ = e;
	}
	count = ps - begin();
}

void selector::querry(const provincei* p) {
	variant owner = p;
	for(auto& e : bsdata<troop>()) {
		if(e.owner == owner)
			add(variant(Troop, bsdata<troop>::source.indexof(&e)));
	}
}