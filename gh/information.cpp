#include "main.h"

static void addh(stringbuilder& sb, const char* title) {
	sb.addn("##%1", title);
}

static bool ismainaction(variant v) {
	switch(v.type) {
	case Action:
		return bsdata<actioni>::get(v.value).ismain();
	case ActionBonus:
		return ismainaction(bsdata<actionbonusi>::get(v.value).action);
	case Feat:
		switch(v.value) {
		case EnemyAttackYouInsteadNearestAlly: return true;
		default: return false;
		}
	case SummonedCreature: case Trap: return true;
	default: return false;
	}
}

static void addpart(stringbuilder& sb, const variants& source) {
	auto auto_new_line = true;
	auto ability_count = 0;
	for(auto v : source) {
		auto action = statable::getaction(v);
		auto bonus = statable::getbonus(v);
		if(action.type==Duration) {
			sb.addn("* ");
			sb.add(action.getname(), bonus);
			sb.add(": ");
			auto_new_line = false;
			continue;
		} else if(ismainaction(action)) {
			if(auto_new_line)
				sb.addn("* ");
		} else
			sb.add(", ");
		if(bonus)
			sb.add("%1 %2i", action.getname(), bonus);
		else if(action.type==SummonedCreature)
			sb.add("%Summon %1", action.getname());
		else
			sb.add(action.getname());
		auto_new_line = true;
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
	}
}