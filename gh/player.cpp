#include "main.h"

void playeri::buildcombatdeck() {
	combat.clear();
	combat.addcombat({});
	combat.shuffle();
}

void playeri::buildhand() {
	variant onwer = this;
	for(auto& e : bsdata<cardi>()) {
		if(e.owner == onwer && e.level == 1)
			hand.add(&e);
	}
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