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

void buildingi::getinfo(stringbuilder& sb) const {
	add_header(sb, id);
	stata stat;
	costa cost;
	for(auto p = this; p; p = p->base)
		p->apply(stat, cost);
	auto m = bsdata<costi>::source.getcount();
	for(unsigned i = 0; i < m; i++) {
		auto v = cost.get(i);
		if(!v)
			continue;
		sb.addn(getnm("RaiseIncomeInProvince"), getnmof(bsdata<costi>::elements[i].id), v);
	}
	m = bsdata<stati>::source.getcount();
	for(unsigned i = 0; i < m; i++) {
		auto v = stat.get(i);
		if(!v)
			continue;
		sb.addn(getnm("RaiseInProvince"), getnm(bsdata<stati>::elements[i].id), v);
	}
	switch(condition.type) {
	case Landscape:
		sb.addn(getnm("CanBuildOnlyIn"), getnmof(bsdata<landscapei>::elements[condition.value].id));
		break;
	}
}

void gamei::getinfo(stringbuilder& sb, const char* id) {
	add_header(sb, id);
	add_description(sb, id);
}

void gamei::getinfov(stringbuilder& sb, variant v) {
	if(!v)
		return;
	buildingi* pb = v;
	if(pb) {
		pb->getinfo(sb);
		return;
	}
	auto id = v.getid();
	add_header(sb, id);
	add_description(sb, id);
}