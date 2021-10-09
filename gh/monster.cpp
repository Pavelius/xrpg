#include "main.h"

int	abilityi::get(action_s v) const {
	switch(v) {
	case Attack: return attack;
	case Move: return move;
	case Range: return range;
	default: return 0;
	}
}

void monsteri::buildcombatdeck() {
	abilities_deck.clear();
	abilities_deck.addcombat8(abilities);
	abilities_deck.shuffle();
}

int monsteri::getinitiative() const {
	return game.getinitiative(abilities_deck.look(0));
}

const abilityi& monsteri::get(int level, bool tought) const {
	if(tought)
		return elite[level];
	return normal[level];
}