#include "main.h"

bool provincei::isbuilded(const buildingi* p) const {
	auto i = bsdata<buildingi>::source.indexof(p);
	if(i == -1)
		return false;
	return buildings.is(i);
}

bool provincei::isupgraded(const buildingi* p) const {
	for(auto& e : bsdata<buildingi>()) {
		if(e.base == p && isbuilded(&e))
			return true;
	}
	return false;
}

static uniti* random(uniti** source4, uniti* def) {
	if(!source4[0])
		return def;
	auto count = 1;
	while(count < 4 && source4[count])
		count++;
	return source4[rand() % count];
}

void provincei::generate_units() {
	army source;
	source.selectall();
	source.shuffle();
	if(!source)
		return;
	auto p = source.data[0].type;
	dwellers = p;
	for(auto count = xrand(1, 4); count > 0; count--)
		garnison.add(p);
}

void provincei::generate_explored() {
	stats.set(Explored, landscape->explored.random());
}

void provincei::generate_population() {
	auto value = landscape->population.random();
	stats.set(Population, value);
}

void provincei::initialize() {
	generate_population();
	generate_explored();
	generate_units();
	update();
}

int provincei::getbuildcount() const {
	auto r = 0;
	for(auto& e : bsdata<buildingi>()) {
		if(!game.province->isbuilded(&e))
			continue;
		if(game.province->isupgraded(&e))
			continue;
		r++;
	}
	return r;
}

bool provincei::build(const buildingi* p, bool run) {
	if(!player)
		return false;
	if(!player->isallow(BuildProvince))
		return false;
	auto i = bsdata<buildingi>::source.indexof(p);
	if(i == -1)
		return false;
	if(isbuilded(p))
		return false;
	if(!ismatch(p->condition))
		return false;
	if(p->base) {
		if(!isbuilded(p->base))
			return false;
	} else {
		auto count = getbuildcount();
		auto count_limit = getbuildlimit();
		if(count >= count_limit)
			return false;
	}
	if(run) {
		buildings.set(i);
		player->actions.add(BuildProvince, -1);
	}
	return true;
}

bool provincei::demontage(const buildingi* p, bool run) {
	if(!player)
		return false;
	if(!player->isallow(DestroyProvince))
		return false;
	auto i = bsdata<buildingi>::source.indexof(p);
	if(i == -1)
		return false;
	if(!isbuilded(p))
		return false;
	if(isupgraded(p))
		return false;
	if(run) {
		buildings.remove(i);
		player->actions.add(DestroyProvince, -1);
	}
	return true;
}

buildingi* provincei::choosebuilding(bool tobuild, bool todemontage) {
	answers an;
	for(auto& e : bsdata<buildingi>()) {
		if(tobuild && !build(&e, false))
			continue;
		if(todemontage && !demontage(&e, false))
			continue;
		an.add(&e, getnm(e.id));
	}
	if(!an)
		return 0;
	return (buildingi*)draw::choosel(an, 0, getnm(id));
}

void provincei::getbuildings(answers* an, stringbuilder* sb) {
	for(auto& e : bsdata<buildingi>()) {
		if(!isbuilded(&e))
			continue;
		if(isupgraded(&e))
			continue;
		if(an)
			an->add(&e, getnm(e.id));
		if(sb)
			e.getpresent(*sb);
	}
}

bool provincei::ismatch(variant v) const {
	switch(v.type) {
	case Landscape: return landscape == v.getpointer();
	default: return true;
	}
}

void provincei::refresh() {
	guard.recover();
	add(Population, get(PopulationGrowth));
}

void provincei::update() {
	op_income = income;
	op_stats = stats;
	op_income += dwellers->nation->income;
	op_stats += dwellers->nation->stats;
	op_income += landscape->income;
	for(auto& e : bsdata<buildingi>()) {
		if(!isbuilded(&e))
			continue;
		e.apply(op_stats, op_income);
	}
}