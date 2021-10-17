#include "main.h"

void army::damage(int count, stringbuilder* sb) {
	for(auto v : *this) {
		if(count <= 0)
			break;
		if(!v)
			continue;
		troop* pt = v;
		if(pt) {
			auto hits = pt->get(Hits, sb);
			if(count > hits) {
				count -= hits;
				pt->kill();
			} else
				break;
		}
	}
}

int army::get(variant v, stringbuilder* sb) const {
	auto r = 0;
	for(auto v : *this) {
		if(!v)
			continue;
		troop* pt = v;
		if(pt)
			r += pt->get(v, sb);
	}
	return r;
}

bool army::conquer(army& enemy, stringbuilder* sb, stat_s attacker_stat, stat_s defender_stat) {
	auto attacker = get(attacker_stat, sb);
	auto defender = enemy.get(defender_stat, sb);
	auto attacker_inflict = get(Damage, sb);
	auto defender_inflict = enemy.get(Damage, sb);
	damage(defender_inflict);
	enemy.damage(attacker_inflict);
	return attacker > defender;
}