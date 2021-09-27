#include "main.h"

static void addh(stringbuilder& sb, const char* title) {
	sb.addn("##%1", title);
}

static variant getduration(const variants& source) {
	for(auto v : source) {
		auto a = statable::getaction(v);
		if(a.type == Duration)
			return v;
	}
	return variant();
}

static void addpart(stringbuilder& sb, const variants& source) {
	int count = 0;
	for(auto v : source) {
		auto action = statable::getaction(v);
		auto bonus = statable::getbonus(v);
		if(action.type==Duration) {
			sb.addn("* ");
			sb.add(action.getname(), bonus);
			sb.add(": ");
			count = -1;
			continue;
		} else if(count < 0)
			count = 0;
		else if(!count)
			sb.addn("* ");
		else
			sb.add(", ");
		if(bonus)
			sb.add("%1 %2i", action.getname(), bonus);
		else if(action.type==SummonedCreature)
			sb.add("%Summon %1", action.getname());
		else
			sb.add(action.getname());
		count++;
	}
}

void cardi::getinfo(stringbuilder& sb) const {
	sb.clear();
	addh(sb, getnm(id));
	sb.addn("%1, %Level %2i, %Initiative %3i", owner.getname(), level, initiative);
	sb.addn("[Верхняя]");
	addpart(sb, upper);
	sb.addn("[Нижняя]");
	addpart(sb, lower);
}

void variant::getinfo(stringbuilder& sb) const {
	switch(type) {
	case Card: bsdata<cardi>::get(value).getinfo(sb); break;
	default: break;
	}
}
