#include "main.h"

BSDATA(actioni) = {
	{"BuildCapital"},
	{"BuildProvince", FG(UseOnProvince)},
	{"ImproveDefence", FG(UseOnProvince)},
};
assert_enum(actioni, ImproveDefence)