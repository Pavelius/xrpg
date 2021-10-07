#include "main.h"

void monsteri::buildcombatdeck() {
	combat.clear();
	combat.addcombat8(abilities);
	combat.shuffle();
}