#include "main.h"

void army::damage(int count, stringbuilder* sb) {
	for(auto& e : *this) {
		if(count <= 0)
			break;
		auto hits = e.get(Hits);
		if(count > hits) {
			count -= hits;
			e.kill();
		} else
			break;
	}
}

int army::get(variant v, stringbuilder* sb) const {
	auto r = 0;
	for(auto& e : *this)
		r += e.get(v, sb);
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