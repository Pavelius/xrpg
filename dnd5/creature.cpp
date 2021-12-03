#include "main.h"

static creature* last_actor;

void creature::clear() {
	memset(this, 0, sizeof(*this));
}

bool creature::is(state_s v) const {
	switch(v) {
	case Alive:
		return hp > 0;
	case Defeated:
		return hp <= 0;
	default:
		return false;
	}
}

void creature::levelup() {
	auto hit_dice = gethd();
	auto& kind = getclass();
	if(ismonster()) {
		basic.hp_maximum += xrand(1, 8);
	} else {
		if(!hit_dice)
			basic.hp_maximum += kind.hd;
		else
			basic.hp_maximum += xrand(1, kind.hd);
	}
	level++;
}

void creature::create_finish() {
	update_finish();
	hp = hp_maximum;
}

void creature::create(racei& race, classi& kind, gender_s gender) {
	clear();
	this->race = &race - bsdata<racei>::elements;
	this->gender = gender;
	this->kind = &kind - bsdata<classi>::elements;
	basic.random_ability(kind);
	levelup();
	create_finish();
}

bool creature::attack(creature& enemy, int advantages, int bonus) {
	auto attack_roll = roll(advantages, is(Lucky)) + bonus;
	auto armor_class = enemy.get(AC);
	return attack_roll >= armor_class;
}

bool creature::attack(ability_s attack_type, creature& enemy, item& weapon, int advantages, int bonus) {
	bonus += get(attack_type);
	fixattack(enemy.getposition(), attack_type);
	if(!attack(enemy, advantages, bonus)) {
		enemy.fixmiss();
		return false;
	}
	auto ai = weapon.geti().attack;
	auto damage_value = roll(ai.damage);
	enemy.damage(ai.type, damage_value);
	return true;
}

void creature::damage(damage_s type, int value) {
	fixdamage(value);
}

void creature::setavatar(const char* v) {
	stringbuilder sb(avatar);
	sb.add(v);
}

static void attack_enemy() {
	last_actor->attack(AttackMelee, bsdata<creature>::get(1), last_actor->get(RightHand), 0, 0);
	draw::waitanimation();
}

static void move_hero() {
	routeto(draw::hilite_index);
	auto pb = indecies.begin();
	for(auto* pi = indecies.end() - 2; pi >= pb; pi--) {
		auto i = *pi;
		last_actor->move(draw::m2s(gx(i), gy(i)));
	}
	draw::refreshmodal();
}

static void fight_proc() {
	last_actor->lookmove();
	answers an;
	an.add(attack_enemy, getnm("Attack"));
	an.modal("What you want to do?", getnm("Cancel"));
}

void creature::fight() {
	last_actor = this;
	draw::modalscene(0, fight_proc, move_hero);
}

void creature::update_finish() {
	auto hit_dice = gethd();
	// If we aware attack add Dexterity bonus
	add(AC, getbonus(Dexterity));
	add(AC, 10);
	add(Speed, 6);
	// After all apply hit points
	hp_maximum += getbonus(Constitution) * hit_dice;
	if(hp_maximum < hit_dice)
		hp_maximum = hit_dice;
}

void creature::update() {
	copy(*this, basic);
}

int creature::gethd() const {
	return level;
}