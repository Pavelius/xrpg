#include "main.h"

deck& creature::getcombatdeck() const {
	return game.enemy_combat;
}