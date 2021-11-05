#include "main.h"

static int compare_units(const void* pv1, const void* pv2) {
	auto p1 = (troop*)pv1;
	auto p2 = (troop*)pv1;
	auto u1 = p1->type;
	auto u2 = p2->type;
	auto a1 = u1->get(Gold);
	auto a2 = u2->get(Gold);
	if(a1 != a2)
		return a2 - a1;
	return 0;
}

void army::sort() {
	qsort(data, count, sizeof(data[0]), compare_units);
}

void army::add(uniti* pu) {
	auto p = adat::add();
	p->clear();
	p->type = pu;
}

void army::shuffle() {
	zshuffle(data, count);
}

void army::select(const landscapei* v) {
	auto i = bsdata<landscapei>::source.indexof(v);
	if(i == -1)
		return;
	for(auto& e : bsdata<uniti>()) {
		if(e.encounter.is(i))
			add(&e);
	}
}

void army::normalize() {
	auto ps = data;
	auto pb = data;
	auto pe = data + count;
	while(pb < pe) {
		if(*pb)
			*ps++ = *pb;
		pb++;
	}
	count = ps - data;
}

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
	normalize();
}

int army::get(variant v, stringbuilder* sb) const {
	auto r = 0;
	for(auto& e : *this)
		r += e.get(v, sb);
	return r;
}

bool army::conquer(army& enemy, stringbuilder* sb) {
	auto attacker = get(Level, sb);
	auto defender = enemy.get(Level, sb);
	auto attacker_inflict = get(Damage, sb);
	auto defender_inflict = enemy.get(Damage, sb);
	damage(defender_inflict);
	enemy.damage(attacker_inflict);
	return attacker > defender;
}

int army::getunitcount(int rang) const {
	auto result = 0;
	for(auto& e : *this) {
		if(e.type->get(Level) == rang)
			result++;
	}
	return result;
}

bool army::is(tag_s v) const {
	for(auto& e : *this) {
		if(e.type->tags.is(v))
			return true;
	}
	return false;
}

int army::getstrenght(bool defensive) const {
	int result = 0;
	for(auto& e : *this)
		result += e.get(Level);
	if(defensive && is(Defensive))
		result++;
	if(!defensive && is(Offensive))
		result++;
	return result;
}