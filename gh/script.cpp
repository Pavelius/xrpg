#include "main.h"

int scripti::getbonus(variant v) {
	switch(v.type) {
	case ActionBonus: return bsdata<actionbonusi>::get(v.value).bonus;
	default: return 0;
	}
}

variant scripti::getaction(variant v) {
	switch(v.type) {
	case ActionBonus: return bsdata<actionbonusi>::get(v.value).action;
	default: return v;
	}
}

void scripti::clear() {
	memset(this, 0, sizeof(*this));
}

static const variant* next_part(const variant* p, const variant* pe) {
	for(; p < pe; p++) {
		auto a = scripti::getaction(*p);
		auto b = scripti::getbonus(*p);
		switch(a.type) {
		case Element:
			if(b < 0)
				return p;
			break;
		case Feat:
			if(a.value == NextAction)
				return p;
			break;
		}
	}
	return p;
}

const variant* scripti::apply(const variant* p, const variant* pe) {
	while(p < pe) {
		auto a = getaction(*p);
		auto b = getbonus(*p);
		switch(a.type) {
		case Action:
			switch(a.value) {
			case Experience: experience += b; break;
			case Pierce: pierce += b; break;
			case Range: range += b; break;
			case Target: targets += b; break;
			default:
				if(action && action != a)
					return p;
				action = a;
				bonus += b;
				break;
			}
			break;
		case Area:
			area = (area_s)a.value;
			area_size += b;
			break;
		case Duration:
			duration = (duration_s)a.value;
			duration_lenght += b;
			break;
		case State:
			states.set(a.value);
			break;
		case Element:
			if(b < 0) {
				if(!game.get(a)) {
					p = next_part(p, pe);
					continue;
				}
				elements_use.set(a.value);
			} else
				elements.set(a.value);
			break;
		case Feat:
			switch(a.value) {
			case NextAction:
				return p + 1;
			default:
				if(action)
					feats.set(a.value);
				else {
					action = a;
					bonus += b;
				}
				break;
			}
			break;
		default:
			if(action && action != a)
				return p;
			action = a;
			bonus += b;
			break;
		}
		p++;
	}
	return p;
}