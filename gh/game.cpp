#include "main.h"

gamei game;

void gamei::add(variant i, int v) {
	set(i, get(i) + v);
}

int gamei::get(variant i) const {
	switch(i.type) {
	case GameProperty:
		if(i.value < sizeof(ability) / sizeof(ability[0]))
			return ability[i.value];
		return 0;
	case Element:
		if(i.value < sizeof(elements) / sizeof(elements[0]))
			return elements[i.value];
		return 0;
	default:
		return 0;
	}
}

bool gamei::isallow(variant v) const {
	switch(v.type) {
	case Player:
		return allowed_players.is(v.value);
	default:
		return false;
	}
}

static object* find_emphty() {
	for(auto& e : bsdata<object>()) {
		if(!e)
			return &e;
	}
	return 0;
}

object* gamei::create(variant v) {
	auto p = find_emphty();
	if(!p)
		p = bsdata<object>::addz();
	p->create(v);
	return p;
}

void gamei::set(variant i, int v) {
	switch(i.type) {
	case GameProperty:
		if(i.value < sizeof(ability) / sizeof(ability[0]))
			ability[i.value] = v;
		break;
	case Element:
		if(i.value < sizeof(elements) / sizeof(elements[0])) {
			elements[i.value] = v;
			if(elements[i.value] > 2)
				elements[i.value] = 2;
			else if(elements[i.value] < 0)
				elements[i.value] = 0;
		}
		break;
	default:
		break;
	}
}

void gamei::buildcombatdeck() {
	enemy_combat.clear();
	enemy_combat.addcombat({});
	enemy_combat.shuffle();
}
