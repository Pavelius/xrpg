#include "main.h"

void selector::match(const landscapei* v, bool keep) {
	auto ps = begin();
	auto id = bsdata<landscapei>::source.indexof(v);
	for(auto e : *this) {
		uniti* pu = e;
		if(!pu)
			continue;
		auto r = pu->encounter.is(id);
		if(r != keep)
			continue;
		*ps++ = e;
	}
	count = ps - begin();
}

void selector::querry(const provincei* p) {
	for(auto& e : bsdata<troop>()) {
		if(e.province == p)
			add(variant(Troop, bsdata<troop>::source.indexof(&e)));
	}
}

static int compare_units(const void* pv1, const void* pv2) {
	auto v1 = *((variant*)pv1);
	auto v2 = *((variant*)pv2);
	troop* p1 = v1;
	troop* p2 = v2;
	if(!p1 || !p2)
		return 0;
	auto u1 = p1->type;
	auto u2 = p2->type;
	auto a1 = u1->get(Gold);
	auto a2 = u2->get(Gold);
	if(a1 != a2)
		return a2 - a1;
	return 0;
}

void selector::sortunits() {
	qsort(data, count, sizeof(data[0]), compare_units);
}