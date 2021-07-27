#include "variant.h"

template<> variant::variant(const void* v) : u(0) {
	for(auto& e : bsdata<varianti>()) {
		if(!e.source)
			continue;
		auto i = e.source->indexof(v);
		if(i != -1) {
			value = i;
			type = (variant_s)(&e - bsdata<varianti>::elements);
			break;
		}
	}
}

template<> variant::variant(const char* v) : u(0) {
	for(auto& e : bsdata<varianti>()) {
		if(!e.source)
			continue;
		auto i = e.source->find(v, 0);
		if(i != -1) {
			value = i;
			type = (variant_s)(&e - bsdata<varianti>::elements);
			break;
		}
	}
}

const char* variant::getdescription() const {
	auto& e = bsdata<varianti>::elements[type];
	if(!e.source)
		return 0;
	return ::getdescription(*((char**)getpointer()));
}

const char* variant::getname() const {
	auto& e = bsdata<varianti>::elements[type];
	if(!e.source)
		return "No name";
	return szt(*((char**)getpointer()));
}