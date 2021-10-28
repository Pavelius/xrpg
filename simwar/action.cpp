#include "main.h"

BSDATA(actioni) = {
	{"BuildProvince"},
	{"DestroyProvince"},
	{"BuildCapital"},
	{"AttackProvince"},
	{"RaidProvince"},
	{"ShowBuildings"},
	{"ShowSites"},
	{"EndTurn"},
	{"CancelAction"},
};
assert_enum(actioni, CancelAction)