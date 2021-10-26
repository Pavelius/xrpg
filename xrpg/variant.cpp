#include "variant.h"

static const char* match(const char* text, const char* name) {
	while(*name) {
		if(*name++ != *text++)
			return 0;
	}
	return text;
}

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
	if(v) {
		for(auto& e : bsdata<varianti>()) {
			if(!e.source || !e.metadata)
				continue;
			int i = -1;
			if(e.isnamed())
				i = e.source->find(v, 0);
			else if(e.is(varianti::FoundByIndex)) {
				auto p = match(v, e.id);
				if(p)
					stringbuilder::read(p, i);
			}
			if(i != -1) {
				value = i;
				type = (variant_s)(&e - bsdata<varianti>::elements);
				break;
			}
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
	return getnm(*((char**)getpointer()));
}

const char* variant::getid() const {
	auto& e = bsdata<varianti>::elements[type];
	if(!e.source)
		return "NoVariant";
	return *((char**)getpointer());
}