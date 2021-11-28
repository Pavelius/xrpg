#include "main.h"

BSDATA(dicei) = {
	{"NoDice"},
	{"D1d2", {1, 2}},
	{"D1d3", {1, 3}},
	{"D1d4", {1, 4}},
	{"D1d6", {1, 6}},
	{"D1d8", {1, 8}},
	{"D1d10", {1, 10}},
	{"D1d12", {1, 12}},
	{"D1d20", {1, 20}},
};
assert_enum(dicei, D1d20)