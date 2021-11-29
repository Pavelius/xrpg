#include "main.h"

static creature* last_actor;

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
	bonus += get(attack_type);
	fixattack(enemy.getposition(), attack_type);
	if(!attack(enemy, advantages, bonus))
		return false;
	auto ai = weapon.geti().attack;
	auto damage_value = roll(ai.damage);
	enemy.damage(ai.type, damage_value);
	return true;
}

void creature::damage(damage_s type, int value) {
}

void creature::setavatar(const char* v) {
	stringbuilder sb(avatar); sb.add(v);
}

static void attack_enemy() {
	last_actor->attack(AttackMelee, bsdata<creature>::get(1), last_actor->get(RightHand), 0, 0);
}

void creature::fight() {
	answers an;
	an.add(attack_enemy, "Attack");
	auto proc = (fnevent)an.choose("What you want to do?", "Cancel", true, 0, 1);
	if(proc) {
		last_actor = this;
		proc();
	}
}