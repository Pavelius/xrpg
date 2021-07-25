#include "bsreq.h"
#include "main.h"

#define VRAR(T) bsdata<T>::source_ptr
#define VRNX(T) {FO(T, name)}
#define VRNT(T) {FO(T, name), FO(T, text)}

BSDATA(varianti) = {
	{""},
	{"Attributes"},
	{"Locations", 0, VRAR(locationi), VRNT(locationi)},
	{"Roles", 0, VRAR(rolei), VRNT(rolei)},
	{"Skills"},
};
assert_enum(varianti, Skill)

BSDATAC(locationi, 128)