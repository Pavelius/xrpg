#include "main.h"

void objects::selectalive() {
	for(auto& e : bsdata<object>()) {
		if(!e || !e.isalive())
			continue;
		add(&e);
	}
}

void objects::select() {
	for(auto& e : bsdata<object>()) {
		if(!e)
			continue;
		add(&e);
	}
}

void objects::selectground() {
	for(auto& e : bsdata<object>()) {
		if(!e || e.isalive())
			continue;
		add(&e);
	}
}

void objects::match(variant v, bool keep) {
	auto ps = begin();
	for(auto p : *this) {
		auto r = (p->get(v) != 0);
		if(r != keep)
			continue;
		*ps++ = p;
	}
	count = ps - begin();
}

void objects::matchrange(int range, bool keep) {
	auto ps = begin();
	for(auto p : *this) {
		auto hex = p2h(p->getposition());
		auto i = h2i(hex);
		auto n = game.getmove(i);
		if(n == Blocked)
			continue;
		auto r = (n <= range);
		if(r != keep)
			continue;
		*ps++ = p;
	}
	count = ps - begin();
}

static int compare(const void* v1, const void* v2) {
	auto p1 = *((object**)v1);
	auto p2 = *((object**)v2);
	return p1->getpriority() - p2->getpriority();
}

void objects::sort() {
	qsort(data, count, sizeof(data[0]), compare);
}

object* objects::choose() const {
	answers an;
	for(auto p : *this)
		an.add((long)p, p->kind.getname());
	return (object*)an.choose(0, 0, true, 0);
}