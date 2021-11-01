#include "main.h"

BSDATA(actioni) = {
	{"BuildProvince"},
	{"DestroyProvince"},
	{"BuildCapital"},
	{"AttackProvince"},
	{"RaidProvince"},
	{"ChooseHeroes"},
	{"ChooseProvinces"},
	{"ShowBuildings"},
	{"ShowSites"},
	{"EndTurn"},
	{"CancelAction"},
};
assert_enum(actioni, CancelAction)