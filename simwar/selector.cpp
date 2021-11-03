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