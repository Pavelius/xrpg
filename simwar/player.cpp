#include "main.h"

BSDATA(playeri) = {
	{"Red"},
	{"Green"},
	{"Yellow"},
	{"Brown"},
	{"Purple"},
	{"Blue"},
	{"Black"},
};
BSDATAF(playeri)

int playeri::get(variant v) const {
	switch(v.type) {
	case Cost: return total.get(v.value);
	default: return 0;
	}
	return 0;
}

void playeri::initialize() {
	total.set(Gold, 1000);
	total.set(Mana, 20);
}

void playeri::refresh() {
	actions.set(BuildProvince, 1);
	actions.set(DestroyProvince, 1);
	actions.set(AttackProvince, 2);
	actions.set(RaidProvince, 1);
	update_resources();
}

void playeri::update_resources() {
	for(auto& e : bsdata<provincei>()) {
		if(e.owner != this)
			continue;
		resources.add(e.resources);
	}
}

bool playeri::hire(uniti* unit, army* garnison, bool run, stringbuilder* sb) {
	if(!unit || !garnison)
		return false;
	if(garnison->getcount() >= garnison->getmaximum()) {
		game.error(sb, "GarnisonCountExceed");
		return false;
	}
	costa unit_cost = unit->cost;
	unit_cost.modify(resources, unit->need);
	if(total < unit_cost) {
		game.error(sb, "NotEnoughtCost");
		return false;
	}
	auto level = unit->get(Level);
	auto leadership = garnison->getleadership();
	auto maximum_count = game.getleadership(leadership, level);
	auto present_count = garnison->getunitcount(level);
	if(present_count + 1 > maximum_count) {
		game.error(sb, "NotEnoughtLeadership");
		return false;
	}
	if(run) {
		total -= unit_cost;
		garnison->add(unit);
	}
	return true;
}