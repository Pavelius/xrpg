#include "main.h"

void playeri::buildcombatdeck() {
	combat.clear();
	combat.addcombat({});
	combat.shuffle();
}

int playeri::get(action_s v) const {
	switch(v) {
	case Coins: return coins;
	case Level: return level;
	case Experience: return exp;
	default: return 0;
	}
}

int playeri::getinitiative() const {
	cardi* p = cards[0];
	if(!p)
		return 99;
	return p->initiative;
}