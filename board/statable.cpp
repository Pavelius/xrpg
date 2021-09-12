#include "tablecsv.h"
#include "main.h"

int statablei::get(variant v) const {
	switch(v.type) {
	case Abilities: return abilities[v.value];
	case Skills: return skills.is(v.value) ? 1 : 0;
	case Feats: return feats.is(v.value) ? 1 : 0;
	default: return 0;
	}
}

void statablei::set(variant i, int v) {
	switch(i.type) {
	case Abilities: abilities[i.value] = v; break;
	case Feats: feats.set(i.value, v != 0); break;
	case Skills: skills.set(i.value, v != 0); break;
	case Records:
		set(bsdata<recordi>::source.records<recordi>().begin()[i.value].parent, v);
		break;
	case Parameters:
		if(bsdata<parameteri>::source.records<parameteri>().begin()[i.value].flags)
			((flagable<1>*)(&parameters[i.value]))->set(v, 1);
		else
			parameters[i.value] = v;
		break;
	}
}

int statablei::geteffect(variants source) const {
	tablecsvi* maptable = 0;
	auto result = 0;
	variant modifier = NoVariant;
	for(auto v : source) {
		if(v.type == Modifiers) {
			modifier = (modifier_s)v.value;
			continue;
		} else if(v.type == Tables) {
			maptable = (tablecsvi*)bsdata<tablecsvi>::source.ptr(v.value);
			continue;
		}
		auto i = get(v);
		if(maptable) {
			i = maptable->get(i);
			maptable = 0;
		}
		if(modifier) {
			auto& em = bsdata<modifieri>::elements[modifier.value];
			switch(em.base) {
			case AbilityBonus: i = i / 2 - 5; break;
			case Minus: i = -i; break;
			case Need: i = (get(i) >= em.value) ? 1 : 0; break;
			}
			modifier = NoVariant;
		}
		result += i;
	}
	return result;
}