#include "main.h"

playeri* army::getplayer() const {
	auto p = getownerhero();
	if(p)
		return p->player;
	auto pr = getownerprovince();
	if(pr)
		return pr->player;
	return 0;
}

void army::selectall() {
	for(auto& e : bsdata<uniti>())
		add(&e);
}

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

int army::damage(int& damage_count, stringbuilder* sb) {
	int result = 0;
	for(auto& e : *this) {
		if(damage_count <= 0)
			break;
		if(!e.survive(e.get(Hits), damage_count)) {
			e.kill(sb);
			result++;
		}
	}
	normalize();
	return result;
}

provincei* army::getownerprovince() const {
	auto i = bsdata<provincei>::source.indexof(this);
	if(i == -1)
		return 0;
	return (provincei*)bsdata<provincei>::source.ptr(i);
}

heroi* army::getownerhero() const {
	auto i = bsdata<heroi>::source.indexof(this);
	if(i == -1)
		return 0;
	return (heroi*)bsdata<heroi>::source.ptr(i);
}

int army::getleadership() const {
	auto pr = getownerprovince();
	if(pr)
		return pr->getleadership();
	auto ph = getownerhero();
	if(ph)
		return ph->getleadership();
	return 0;
}

int army::get(variant v, stringbuilder* sb) const {
	auto r = 0;
	for(auto& e : *this)
		r += e.get(v, sb);
	return r;
}

int army::getlimited(variant v, int limit, stringbuilder* sb) const {
	auto r = get(v, 0);
	if(r > limit)
		r = limit;
	return r;
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
	auto result = get(Level);
	if(defensive) {
		result += getlimited(Defensive, 3);
	} else {
		result += getlimited(Offensive, 3);
	}
	return result;
}

const tactici* army::gettactic() const {
	tactici* result = 0;
	auto ph = getownerhero();
	if(ph)
		result = ph->tactic;
	return result;
}

static int army_apply_damage(army* p, int& hits, stringbuilder* sb, const char* tips_string, playeri* side) {
	char temp[1024]; temp[0] = 0;
	stringbuilder sbd(temp);
	auto result = p->damage(hits, &sbd);
	if(sb)
		sb->addn(getdescription("Casualty"), game.getnameof(side), tips_string, temp[0] ? temp : getnm("No"));
	return result;
}

bool army::conquer(army& enemy, stringbuilder* sb) {
	game.attacker = this;
	game.garnison = &enemy;
	if(sb) {
		sb->adds(getnm("AttackerPromt"));
		sb->adds(getnm("DefenderPromt"));
	}
	auto attacker_side = getplayer();
	auto defender_side = enemy.getplayer();
	auto attacker_tactic = gettactic();
	auto defender_tactic = enemy.gettactic();
	if(attacker_tactic == defender_tactic) {
		if(sb)
			sb->addn(getdescription("NoTactic"));
	}
	char attacker_temp[1024]; stringbuilder asb(attacker_temp); attacker_temp[0] = 0;
	char defender_temp[1024]; stringbuilder dsb(defender_temp); defender_temp[0] = 0;
	auto attacker_inflict = get(Damage, &asb);
	auto defender_inflict = enemy.get(Damage, &dsb);
	auto attacker_killed = army_apply_damage(this, defender_inflict, sb, defender_temp, attacker_side);
	auto defender_killed = army_apply_damage(&enemy, attacker_inflict, sb, attacker_temp, defender_side);
	auto attacker = get(Level, 0);
	auto defender = enemy.get(Level, 0);
	damage(defender_inflict);
	enemy.damage(attacker_inflict);
	return attacker > defender;
}