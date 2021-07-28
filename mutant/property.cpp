#include "bsreq.h"
#include "main.h"

#define VRAR(T) bsdata<T>::source_ptr

BSDATA(varianti) = {
	{""},
	{"Attributes", 0, VRAR(attributei)},
	{"Items", 0, VRAR(itemi)},
	{"Genders", 0, VRAR(genderi)},
	{"Locations", 0, VRAR(locationi)},
	{"Roles", 0, VRAR(rolei)},
	{"Skills", 0, VRAR(skilli)},
	{"Talents", 0, VRAR(talenti)},
};
assert_enum(varianti, Talent)

BSDATAC(character, 128)
BSDATAC(locationi, 128)