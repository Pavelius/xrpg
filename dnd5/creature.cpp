#include "main.h"

static creature* last_attacker;

void creature::clear() {
	memset(this, 0, sizeof(*this));
}

void creature::create(racei& race, classi& kind, gender_s gender) {
	clear();
	this->race = &race - bsdata<racei>::elements;
	this->gender = gender;
	basic.random_ability(kind);
}

bool creature::attack(creature& enemy, int advantages, int bonus) {
	auto attack_roll = roll(advantages, is(Lucky)) + bonus;
	auto armor_class = enemy.get(AC);
	return attack_roll >= armor_class;
}

bool creature::attack(ability_s attack_type, creature& enemy, item& weapon, int advantages, int bonus) {
	last_attacker = this;
	bonus += get(attack_type);
	if(!attack(enemy, advantages, bonus)) {
		fixmiss();
		return false;
	}
	auto ai = weapon.geti().attack;
	auto damage_value = roll(ai.damage);
	enemy.damage(ai.type, damage_value);
	return true;
}

void creature::damage(damage_s type, int value) {
}

void creature::fixmiss() {
}

void creature::fixhit(int value) {
}