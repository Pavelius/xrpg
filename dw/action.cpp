#include "main.h"

BSDATA(actioni) = {
	{"SufferDamage"},
	{"InflictDamage"},
	{"UseAmmo"},
	{"UseGear"},
	{"UseRation"},
	{"ForgetSpell"},
	{"BreakItem"},
	{"BreakObject"},
};
assert_enum(actioni, BreakObject)