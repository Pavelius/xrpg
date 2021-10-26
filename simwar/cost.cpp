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
	auto a = game.getaction(v);
	auto b = game.getbonus(v);
	if(a.type == Cost)
		return;
	add(v.value, b);
}