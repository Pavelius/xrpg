#include "main.h"

int statable::getbonus(variant v) {
	switch(v.type) {
	case ActionBonus: return bsdata<actionbonusi>::get(v.value).bonus;
	default: return 0;
	}
}

variant statable::getaction(variant v) {
	switch(v.type) {
	case ActionBonus: return bsdata<actionbonusi>::get(v.value).action;
	default: return v;
	}
}

int statable::get(variant i) const {
	switch(i.type) {
	case Action: return actions[i.value];
	case State: return states.is(i.value) ? 1 : 0;
	default: return 0;
	}
}

void statable::set(variant i, int v) {
	switch(i.type) {
	case Action: actions[i.value] = v; break;
	case State: states.set(i.value); break;
	default: break;
	}
}

void statable::apply(variant v) {
	auto a = getaction(v);
	auto b = getbonus(v);
	switch(v.type) {
	case State:
		if(!b)
			b = 1;
		break;
    default: break;
	}
	set(a, get(a) + b);
}
