#include "main.h"

BSDATA(durationi) = {
	{"Instant"},
	{"Round"},
	{"PerUse"},
	{"Infinite"},
};
assert_enum(durationi, Infinite)