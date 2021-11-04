#include "main.h"

BSDATA(actioni) = {
	{"CancelAction", gamei::playermove},
	{"BuildProvince", gamei::build},
	{"DestroyProvince", gamei::demontage},
	{"BuildCapital"},
	{"AttackProvince"},
	{"RaidProvince"},
	{"RecruitUnits", gamei::recruit},
	{"ChooseHeroes"},
	{"ChooseProvinces"},
	{"ShowBuildings", gamei::buildings},
	{"ShowSites"},
	{"EndTurn", gamei::nextmove},
};
assert_enum(actioni, EndTurn)