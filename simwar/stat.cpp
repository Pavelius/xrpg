#include "main.h"

BSDATA(stati) = {
	{"Attack"},
	{"Defend"},
	{"Raid"},
	{"Move"},
	{"Damage"},
	{"Hits"},
	{"Level"},
};
assert_enum(stati, Level)