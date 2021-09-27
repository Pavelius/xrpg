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

static modifier_s getmodifier(variant v) {
    switch(v.type) {
    case Action: return bsdata<actioni>::get(v.value).type;
    case Area: return Modifier;
    case Duration: return Modifier;
    default: return MainAction;
    }
}

const variant* statable::parse(const variant* p, const variant* pe) {
    variant main_action, area, duration;
    for(;p < pe; p++) {
        auto a = getaction(*p);
        switch(a.type) {
        case Duration:
            duration = *p;
            continue;
        case Area:
            area = *p;
            continue;
        default:
            break;
        }
        if(main_action)
            break;
        auto b = getbonus(*p);
    }
    return p;
}