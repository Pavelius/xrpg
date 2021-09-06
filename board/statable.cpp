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