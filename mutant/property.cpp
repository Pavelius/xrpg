#include "bsreq.h"
#include "main.h"

BSDATA(varianti) = {
	{""},
	{"Attributes"},
	{"Locations", 0, bsdata<locationi>::source_ptr, {FO(locationi, name)}},
	{"Skills"},
};
assert_enum(varianti, Skill)

BSDATAC(locationi, 128)