#include "main.h"

bool creature::attack(creature& enemy, wear_s slot, int advantages, int bonus) {
	auto attack_roll = roll(advantages, is(Lucky)) + bonus;
	auto armor_class = enemy.get(AC);
	return attack_roll >= armor_class;
}