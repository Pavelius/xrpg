#include "main.h"

BSDATA(actioni) = {
	{"BuildProvince"},
	{"DestroyProvince"},
	{"BuildCapital"},
	{"AttackProvince"},
	{"RaidProvince"},
};
assert_enum(actioni, RaidProvince)