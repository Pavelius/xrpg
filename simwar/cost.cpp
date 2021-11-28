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
	auto a = v.getaction();
	auto b = v.getbonus();
	if(a.type == Cost)
		return;
	add(v.value, b);
}

void costa::modify(const resourcea& allowed, const producea& need) {
	for(auto v : need) {
		auto p = bsdata<resourcei>::elements + v;
		if(allowed.is(v))
			continue;
		*this += p->cost;
	}
}