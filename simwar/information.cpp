#include "main.h"

static void add_header(stringbuilder& sb, const char* id) {
	sb.addn("###%1", getnm(id));
}

static void add_description(stringbuilder& sb, const char* id) {
	auto p = getdescription(id);
	if(!p)
		return;
	if(equal(id, "Date"))
		sb.addn(p, game.getyear(), game.getturn());
	else
		sb.addn(p);
}

static void add_line(stringbuilder& sb, const stata& source) {
	auto m = bsdata<stati>::source.getcount();
	for(unsigned i = 0; i < m; i++) {
		if(i == Hits || i==Level)
			continue;
		auto v = source.get(i);
		if(!v)
			continue;
		if(v>0)
			sb.addn("[+%+2i]\t%1", getnm(bsdata<stati>::get(i).id), v);
		else
			sb.addn("[-%+2i]\t%1", getnm(bsdata<stati>::get(i).id), v);
	}
}

static void add_cost(stringbuilder& sb, const costa& source) {
	auto m = bsdata<costi>::source.getcount();
	auto p = sb.get();
	for(unsigned i = 0; i < m; i++) {
		auto v = source.get(i);
		if(!v)
			continue;
		if(p == sb.get())
			sb.addn("%Price: ");
		sb.add("%2i:%1i:", i, v);
	}
}

static void add_bonuses(stringbuilder& sb, const char* id, const costa& source) {
	auto m = bsdata<costi>::source.getcount();
	for(unsigned i = 0; i < m; i++) {
		auto v = source.get(i);
		if(!v)
			continue;
		sb.addn(getnm(id), getnmof(bsdata<costi>::elements[i].id), v);
	}
}

static void add_bonuses(stringbuilder& sb, const char* id, const stata& source) {
	auto m = bsdata<stati>::source.getcount();
	for(unsigned i = 0; i < m; i++) {
		auto v = source.get(i);
		if(!v)
			continue;
		sb.addn(getnm(id), getnm(bsdata<stati>::elements[i].id), v);
	}
}

static void add_bonuses(stringbuilder& sb, const stata& source) {
	auto m = bsdata<stati>::source.getcount();
	auto p = sb.get();
	for(unsigned i = 0; i < m; i++) {
		auto v = source.get(i);
		if(!v)
			continue;
		if(p != sb.get())
			sb.add(", ");
		else
			sb.add("\n");
		sb.add("%1 %2i", getnm(bsdata<stati>::elements[i].id), v);
	}
}

void provincei::getpresent(stringbuilder& sb) const {
	sb.add("$left image %1 0 \"art/images\" \"%1\"\n", landscape->id);
	sb.add("$right image %1 0 \"art/images\" \"%1\"\n", dwellers->nation->id);
	sb.add("$progress \"%Explored %1i%%\" 0 100 %1i \"Explored\"\n", get(Explored));
	auto population = get(Population);
	auto pp = populationi::findbypopulation(population);
	auto range = pp->getrange();
	auto percent = range.percent(population);
	sb.add("$progress \"%4\" %2i %3i %1i \"Population\"\n", population, range.min, range.max, getnm(pp->id));
	sb.add("$end\n");
}

void buildingi::getpresent(stringbuilder& sb) const {
	sb.add("$left image %1 0 \"art/images\" \"%1\"\n", id);
}

void hero::getinfo(stringbuilder& sb) const {
	//sb.addn("$left image %1 0 \"art/images\" \"@%2\"", id, id);
	sb.addn("###%1", getnm(id));
	add_line(sb, stats);
}

void tactici::getinfo(stringbuilder& sb) const {
	add_header(sb, id);
	stata stat; costa cost;
	for(auto v : bonus)
		game.apply(v, stat, cost);
	add_bonuses(sb, "RaiseInProvince", stat);
}

void buildingi::getinfo(stringbuilder& sb, bool show_cost) const {
	getpresent(sb);
	add_header(sb, id);
	stata stat; costa cost;
	for(auto p = this; p; p = p->base)
		p->apply(stat, cost);
	add_bonuses(sb, "RaiseIncomeInProvince", cost);
	add_bonuses(sb, "RaiseInProvince", stat);
	switch(condition.type) {
	case Landscape:
		sb.addn(getnm("CanBuildOnlyIn"), getnmof(bsdata<landscapei>::elements[condition.value].id));
		break;
	}
	if(show_cost)
		add_cost(sb, this->cost);
}

void landscapei::getinfo(stringbuilder& sb) const {
	add_header(sb, id);
	add_bonuses(sb, "RaiseIncomeInProvince", income);
}

void gamei::getinfo(stringbuilder& sb, const char* id) {
	variant v = (const char*)(id);
	if(v)
		v.getinfo(sb);
	else {
		add_header(sb, id);
		add_description(sb, id);
	}
}

void variant::getinfo(stringbuilder& sb) const {
	if(type==NoVariant)
		return;
	buildingi* pb = *this;
	if(pb) {
		pb->getinfo(sb, true);
		return;
	}
	tactici* pt = *this;
	if(pt) {
		pt->getinfo(sb);
		return;
	}
	landscapei* pl = *this;
	if(pl) {
		pl->getinfo(sb);
		return;
	}
	hero* ph = *this;
	if(ph) {
		ph->getinfo(sb);
		return;
	}
	auto id = getid();
	add_header(sb, id);
	add_description(sb, id);
}