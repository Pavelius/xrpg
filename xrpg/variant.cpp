#include "variant.h"

variant::variant(const void* v) : u(0) {
	for(auto& e : bsdata<varianti>()) {
		if(!e.source)
			continue;
		auto i = e.source->indexof(v);
		if(i != -1) {
			u = i;
			c[3] = &e - bsdata<varianti>::elements;
			break;
		}
	}
}

variant::variant(const char* v) : u(0) {
	for(auto& e : bsdata<varianti>()) {
		if(!e.source || !e.names[0])
			continue;
		auto i = e.source->find(v, 0);
		if(i != -1) {
			u = i;
			c[3] = &e - bsdata<varianti>::elements;
			break;
		}
	}
}

const char* variant::getname() const {
	auto& e = bsdata<varianti>::elements[getkind()];
	if(!e.names[0])
		return "No name";
	return *(const char**)((char*)getpointer() + e.names[0]);
}