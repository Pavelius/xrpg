#include "main.h"

BSDATA(actioni) = {
	{"BuildProvince"},
	{"DestroyProvince"},
	{"BuildCapital"},
	{"AttackProvince"},
	{"RaidProvince"},
	{"RecruitUnits"},
	{"ChooseHeroes"},
	{"ChooseProvinces"},
	{"ShowBuildings"},
	{"ShowSites"},
	{"EndTurn"},
	{"CancelAction"},
};
assert_enum(actioni, CancelAction)