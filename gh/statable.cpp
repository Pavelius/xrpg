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
	case Feat: return feats.is(i.value) ? 1 : 0;
	default: return 0;
	}
}

void statable::set(variant i, int v) {
	switch(i.type) {
	case Action: actions[i.value] = v; break;
	case Feat: feats.set(i.value, v != 0); break;
	case State: states.set(i.value, v != 0); break;
	default: break;
	}
}

static const variant* read_one_action(statable* ps, const variant* p, const variant* pe) {
	variant duration, area, main_action;
	for(; p < pe; p++) {
		auto v = *p;
		auto a = ps->getaction(v);
		if(v == variant(NextAction)) {
			p++;
			break;
		}
		switch(a.type) {
		case Duration:
			duration = v;
			continue;
		case Area:
			area = v;
			continue;
		default:
			break;
		}
		auto b = ps->getbonus(v);
		if(!b)
			b = 1;
		if(!main_action)
			main_action = v;
		ps->set(v, ps->get(v) + b);
	}
	return p;
}

void statable::apply(const variants& source) {
	variant duration, area, main_action;
	for(auto v : source) {
		auto a = getaction(v);
		switch(a.type) {
		case Duration:
			duration = v;
			continue;
		case Area:
			area = v;
			continue;
		default:
			break;
		}
		auto b = getbonus(v);
		if(!b)
			b = 1;
		if(!main_action)
			main_action = v;
		set(v, get(v) + b);
	}
}