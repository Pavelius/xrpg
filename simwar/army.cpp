#include "main.h"

static int compare_units(const void* pv1, const void* pv2) {
	auto p1 = (troop*)pv1;
	auto p2 = (troop*)pv1;
	auto u1 = p1->type;
	auto u2 = p2->type;
	auto a1 = u1->get(Gold);
	auto a2 = u2->get(Gold);
	if(a1 != a2)
		return a2 - a1;
	return 0;
}

void army::sort() {
	qsort(data, count, sizeof(data[0]), compare_units);
}

void army::add(uniti* pu) {
	auto p = adat::add();
	p->clear();
	p->type = pu;
}

void army::shuffle() {
	zshuffle(data, count);
}

void army::select(const landscapei* v) {
	auto i = bsdata<landscapei>::source.indexof(v);
	if(i == -1)
		return;
	for(auto& e : bsdata<uniti>()) {
		if(e.encounter.is(i))
			add(&e);
	}
}