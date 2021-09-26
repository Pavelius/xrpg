#include "main.h"

static void addcards(combatdeck& source, variant owner) {
	for(auto& e : bsdata<combatcardi>()) {
		if(e.owner != owner)
			continue;
		for(auto i = 0; i < e.count; i++)
			source.add(&e);
	}
}

void playeri::buildcombatdeck() {
	combat.clear();
	addcards(combat, {});
	zshuffle(combat.data, combat.count);
}