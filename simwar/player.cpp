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