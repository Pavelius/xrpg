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
	selector source;
	source.select(bsdata<uniti>::source);
	source.match(landscape, true);
	source.shuffle();
	if(!source)
		return;
	uniti* p = source.data[0];
	if(!p)
		return;
	dwellers = p;
	for(auto count = xrand(1, 4); count > 0; count--) {
		if(chance(75) || count == 1)
			game.addtroop(p, this);
		else if(chance(75))
			game.addtroop(random(p->encounter_tought, p), this);
		else
			game.addtroop(random(p->encounter_monster, p), this);
	}
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
	if(!owner)
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
	if(run)
		buildings.set(i);
	return true;
}

bool provincei::demontage(const buildingi* p, bool run) {
	auto i = bsdata<buildingi>::source.indexof(p);
	if(i == -1)
		return false;
	if(!isbuilded(p))
		return false;
	if(isupgraded(p))
		return false;
	if(run)
		buildings.remove(i);
	return true;
}

void provincei::addto(answers& an) const {
	an.add((long)this,
		"#$left image %1 0 \"art/images\" \"@%1\"\n"
		"$right image %1 0 \"art/images\" \"@%1\"\n",
		landscape->id,
		dwellers->id,
		id, getnm(id));
}

void provincei::canbuild(answers& an) {
	for(auto& e : bsdata<buildingi>()) {
		if(!build(&e, false))
			continue;
		an.add((long)&e, getnm(e.id));
	}
}

void provincei::getbuildings(answers* an, stringbuilder* sb) {
	for(auto& e : bsdata<buildingi>()) {
		if(!isbuilded(&e))
			continue;
		if(isupgraded(&e))
			continue;
		if(an)
			an->add((long)&e, getnm(e.id));
		if(sb)
			e.getpresent(*sb);
	}
}

void provincei::update() {
	for(auto& e : bsdata<buildingi>()) {
		if(!isbuilded(&e))
			continue;
		for(auto v : e.effect)
			game.apply(v, stats, income);
	}
}

bool provincei::ismatch(variant v) const {
	switch(v.type) {
	case Landscape: return landscape == v.getpointer();
	default: return true;
	}
}