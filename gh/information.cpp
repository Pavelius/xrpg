#include "main.h"

static void addh(stringbuilder& sb, const char* title) {
	sb.addn("##%1", title);
}

static void addline(stringbuilder& sb) {
	sb.addn("---");
}

static bool ismainaction(variant v) {
	if(v.type == ActionBonus) {
		auto& ab = bsdata<actionbonusi>::get(v.value);
		if(ab.action.type == Action)
			return bsdata<actioni>::get(ab.action.value).ismain();
	}
	return false;
}

static void addpart(stringbuilder& sb, const variants& source) {
	for(auto v : source) {
		if(v.type == ActionBonus) {
			auto& ab = bsdata<actionbonusi>::get(v.value);
			if(ismainaction(v)) {
				sb.addn("%1 %2i", ab.action.getname(), ab.bonus);
			} else {
				sb.add(", %1 %2i", ab.action.getname(), ab.bonus);
			}
		}
	}
}

void cardi::getinfo(stringbuilder& sb) const {
	sb.clear();
	addh(sb, getnm(id));
	sb.addn("%1, %Level %2i, %Initiative %3i", owner.getname(), level, initiative);
	addpart(sb, upper);
	addline(sb);
	addpart(sb, lower);
}

void variant::getinfo(stringbuilder& sb) const {
	switch(type) {
	case Card: bsdata<cardi>::get(value).getinfo(sb); break;
	}
}