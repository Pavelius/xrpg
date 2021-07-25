#include "bsreq.h"
#include "main.h"

#define VRAR(T) bsdata<T>::source_ptr
#define VRNT(T) {FO(T, name), 0, 0, FO(T, text)}

BSDATA(varianti) = {
	{""},
	{"Attributes"},
	{"Items", 0, VRAR(itemi), VRNT(itemi)},
	{"Genders", 0, VRAR(genderi), VRNT(genderi)},
	{"Locations", 0, VRAR(locationi), VRNT(locationi)},
	{"Roles", 0, VRAR(rolei), VRNT(rolei)},
	{"Skills"},
};
assert_enum(varianti, Skill)

BSDATAC(character, 128)
BSDATAC(locationi, 128)