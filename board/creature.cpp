#include "main.h"

static creaturei*		current;
typedef bool(fnchooseallow)(const void* p);

void creaturei::clear() {
	memset(this, 0, sizeof(*this));
}

static bool allow(const char* type, const char* id) {
	if(equal(type, "Genders")) {
		if(equal(id, "NoGender"))
			return false;
	}
	return true;
}

static void fill_array(answers& an, array* source, fnchooseallow pallow) {
	const char* pe = source->end();
	for(auto pb = source->begin(); pb < pe; pb += source->size) {
		if(pallow) {
			if(!pallow(pb))
				continue;
		}
		auto id = *((const char**)pb);
		an.add((long)pb, getnm(id));
	}
}

static void fill_records(answers& an, variant source, fnchooseallow pallow) {
	for(auto& e : bsdata<recordi>()) {
		if(e.parent != source)
			continue;
		if(pallow) {
			if(!pallow(&e))
				continue;
		}
		an.add((long)&e, getnm(e.id));
	}
}

static void fill_source(answers& an, variant source, fnchooseallow pallow) {
	switch(source.type) {
	case Types:
		fill_array(an, bsdata<varianti>::elements[source.value].source, pallow);
		break;
	case Records:
		fill_records(an, ((recordi*)bsdata<recordi>::source.ptr(source.value))->parent, pallow);
		break;
	default:
		fill_records(an, source, pallow);
		break;
	}
}

static variant param1, param2, result;

static void choose_source(bool interactive, const char* title, const char* resid, variant source, fnchooseallow pallow = 0) {
	answers an;
	fill_source(an, source, pallow);
	result = (void*)an.choose(getnm(title), 0, interactive, resid);
}

static bool allow_gender(const void* p) {
	return p != bsdata<genderi>::begin();
}

static bool allow_base_race(const void* p) {
	return !((racei*)p)->base.operator bool();
}

static bool allow_subrace(const void* p) {
	return ((racei*)p)->base == param1;
}

static fnchooseallow* getallow(const char* id) {
	if(equal("ChooseRace", id))
		return allow_base_race;
	if(equal("ChooseSubRace", id)) {
		param1 = result;
		return allow_subrace;
	}
	if(equal("ChooseGender", id))
		return allow_gender;
	return 0;
}

void create_character() {
	auto interactive = true;
	for(auto& e : bsdata<inputi>()) {
		if(!e.id || !e.source)
			break;
		choose_source(interactive, e.id, e.resid, e.source, getallow(e.id));
	}
}