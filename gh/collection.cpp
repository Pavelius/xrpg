#include "main.h"

static bool matchv(variant v, variant e) {
	switch(v.type) {
	case Object: return bsdata<object>::get(v.value).get(e) != 0;
	default: return false;
	}
	return false;
}

void collection::selectalive() {
	auto p = data;
	auto pe = endof();
	for(auto& e : bsdata<object>()) {
		if(!e.isalive())
			continue;
		*p++ = &e;
	}
	count = p - data;
}

void collection::match(variant e, bool keep) {
	auto p = begin();
	auto pe = end();
	for(auto v : *this) {
		auto r = matchv(v, e);
		if(r != keep)
			continue;
		*p++ = v;
	}
	count = p - begin();
}

void collection::matchhostile(bool keep) {
	auto p = begin();
	auto pe = end();
	for(auto v : *this) {
		object* ps = v;
		if(!ps)
			continue;
		if(ps->is(Hostile) != keep)
			continue;
		*p++ = v;
	}
	count = p - begin();
}