#include "variant.h"

static const char* match(const char* text, const char* name) {
	while(*name) {
		if(*name++ != *text++)
			return 0;
	}
	return text;
}

const char* variant::getdescription() const {
	auto& e = bsdata<varianti>::elements[type];
	if(!e.source)
		return 0;
	return ::getdescription(getid());
}

const char* variant::getname() const {
	auto& e = bsdata<varianti>::elements[type];
	if(!e.source)
		return "No name";
	return getnm(getid());
}

const char* variant::getid() const {
	auto& e = bsdata<varianti>::elements[type];
	if(!e.source)
		return "NoVariant";
	return *((char**)getpointer());
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

int varianti::found(const char* id) const {
	int i = -1;
	if(isnamed())
		i = source->find(id, 0);
	else if(is(varianti::FoundByIndex)) {
		auto p = match(id, this->id);
		if(p)
			stringbuilder::read(p, i);
	}
	return i;
}

template<> variant::variant(const char* v) : u(0) {
	if(v) {
		for(auto& e : bsdata<varianti>()) {
			if(!e.source || !e.metadata)
				continue;
			int i = e.found(v);
			if(i != -1) {
				value = i;
				type = (variant_s)(&e - bsdata<varianti>::elements);
				break;
			}
		}
	}
}