#include "main.h"

int summary::getbonus(variant v) {
	switch(v.type) {
	case ActionBonus: return bsdata<actionbonusi>::get(v.value).bonus;
	default: return 0;
	}
}

variant summary::getaction(variant v) {
	switch(v.type) {
	case ActionBonus: return bsdata<actionbonusi>::get(v.value).action;
	default: return v;
	}
}

int summary::get(variant i) const {
	switch(i.type) {
	case Action: return actions[i.value];
	case State: return states.is(i.value) ? 1 : 0;
	case Feat: return feats.is(i.value) ? 1 : 0;
	default: return 0;
	}
}

void summary::set(variant i, int v) {
	switch(i.type) {
	case Action: actions[i.value] = v; break;
	case Feat: feats.set(i.value, v != 0); break;
	case State: states.set(i.value, v != 0); break;
	default: break;
	}
}

void summary::apply(variant v) {
	auto a = getaction(v);
	auto b = getbonus(v);
	switch(a.type) {
	case Duration:
		duration = (duration_s)a.value;
		duration_bonus = b;
		break;
	case Area:
		area = (area_s)a.value;
		area_bonus = b;
		break;
	case Feat:
		break;
	default:
        if(!b)
            b = 1;
        set(v, get(v) + b);
		break;
    }
}

void summary::apply(const variants& source) {
	for(auto v : source) {

	}
}