#include "main.h"

BSDATA(actioni) = {
	{"Shield"},
	{"Retaliate"},
	{"Move"},
	{"Attack"},
	{"Push"},
	{"Pull"},
	{"Heal"},
	{"DisarmTrap"},
	{"Loot"},
	{"Range"},
	{"Target"},
	{"Pierce"},
	{"Bless"},
	{"Curse"},
	{"RecoverDiscarded"},
	{"Experience"},
	{"Level"},
	{"Pay"},
	{"ChooseCoinOrReputation"},
	{"Shuffle"},
	{"NextAction"},
};
assert_enum(actioni, NextAction)