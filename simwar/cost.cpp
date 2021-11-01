#include "main.h"

BSDATA(costi) = {
	{"Gold", true},
	{"Mana", true},
	{"Artifacts", true},
	{"Faith"},
	{"Fame"},
};
assert_enum(costi, Fame)

void costa::apply(variant v, const prefixa& flags) {
	auto a = bonusi::getaction(v);
	auto b = bonusi::getbonus(v);
	if(a.type == Cost)
		return;
	add(v.value, b);
}