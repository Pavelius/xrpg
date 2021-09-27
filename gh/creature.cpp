#include "main.h"

deck& creature::getcombatdeck() const {
	return game.enemy_combat;
}

void creature::heal(int v) {
}

void creature::move(int v) {
}

void creature::attack(int damage, int range, int pierce, statef states) {
}

void creature::pull(int range) {
}

void creature::push(int range) {
}

void creature::apply(variant v, int bonus) {
	switch(v.type) {
	case Action:
		switch(v.value) {
		case Attack:
			attack(bonus, get(Range), get(Pierce), getstates());
			break;
		case Move:
			move(bonus);
			break;
		case Heal:
			heal(bonus);
			break;
		case Push:
			push(bonus);
			break;
		case Pull:
			pull(bonus);
			break;
		default:
			if(!bonus)
				bonus = 1;
			set(v, get(v) + bonus);
			break;
		}
		break;
	}
}