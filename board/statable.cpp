#include "tablecsv.h"
#include "main.h"

void statablei::add(variant v) {
	switch(v.type) {
	case Abilities:
		abilities[v.value]++;
		break;
	case Feats:
		feats.set(v.value);
		break;
	case Skills:
		skills.set(v.value);
		break;
	case Races:
		for(auto e : bsdata<racei>::elements[v.value].feats)
			add(e);
		break;
	}
}

int statablei::get(variant v) const {
	switch(v.type) {
	case Abilities: return abilities[v.value];
	case Skills: return skills.is(v.value) ? 1 : 0;
	case Feats: return feats.is(v.value) ? 1 : 0;
	default: return 0;
	}
}

int statablei::geteffect(variants source) const {
	tablecsvi* maptable = 0;
	auto result = 0;
	auto modifier = NoModifier;
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
		switch(modifier) {
		case AbilityBonus: i = i / 2 - 5; break;
		case Minus: i = -i; break;
		}
		modifier = NoModifier;
		result += i;
	}
	return result;
}