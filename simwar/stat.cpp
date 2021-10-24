#include "main.h"

BSDATA(stati) = {
	{"Attack"},
	{"Defend"},
	{"Raid"},
	{"Move"},
	{"Damage"},
	{"Hits"},
	{"Level"},
	{"Explored"},
	{"Population"},
	{"PopulationGrowth"},
	{"Rebellion"},
	{"Happiness"},
};
assert_enum(stati, Happiness)