#include "log.h"
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
		if(!e.source || !e.locale[0])
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
	const int text_index = sizeof(varianti::locale) / sizeof(varianti::locale[0]) - 1;
	auto& e = bsdata<varianti>::elements[type];
	if(!e.locale[text_index])
		return 0;
	return *(const char**)((char*)getpointer() + e.locale[text_index]);
}

const char* variant::getname() const {
	auto& e = bsdata<varianti>::elements[type];
	if(!e.locale[0])
		return "No name";
	return *(const char**)((char*)getpointer() + e.locale[0]);
}

bool savel(const char* url, array& source, unsigned* fields, int fields_count, unsigned* special, int special_count);
bool readl(const char* url, array& source, unsigned* fields, int fields_count, unsigned* special, int special_count);

static void checklocalization(varianti& e, const char* url) {
	for(unsigned i = 0; i < e.source->getcount(); i++) {
		auto p = (char*)e.source->ptr(i);
		auto id = *((const char**)(p));
		auto p1 = *((const char**)(p + e.locale[0]));
		if(!p1 || !p1[0]) {
			if(url) {
				log::error("Errors in file %1.txt", url);
				url = 0;
			}
			log::error("Missing name for '%1'", id);
		}
	}
}

bool varianti::localization(const char* locale_id, bool writemode) {
	const auto fields_count = sizeof(varianti::locale) / sizeof(varianti::locale[0]);
	for(auto& e : bsdata<varianti>()) {
		if(!e.source || !e.locale[0])
			continue;
		char temp[260]; stringbuilder sb(temp);
		sb.add("locale/%1/%2.txt", locale_id, e.id);
		auto special_count = e.special ? zlen(e.special) : 0;
		if(writemode)
			savel(temp, *e.source, e.locale, fields_count, e.special, special_count);
		else {
			if(!readl(temp, *e.source, e.locale, fields_count, e.special, special_count))
				log::error("Can't find file %1", temp);
			else
				checklocalization(e, temp);
		}
	}
	return log::geterrors()==0;
}