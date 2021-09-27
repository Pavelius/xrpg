#include "main.h"

gamei game;

void gamei::add(variant i, int v) {
	set(i, get(i) + v);
}

int gamei::get(variant i) const {
	switch(i.type) {
	case GameProperty:
        if(i.value<sizeof(ability)/sizeof(ability[0]))
            return ability[i.value];
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

void gamei::set(variant i, int v) {
	switch(i.type) {
	case GameProperty:
		if(i.value<sizeof(ability)/ sizeof(ability[0]))
			ability[i.value] = v;
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
