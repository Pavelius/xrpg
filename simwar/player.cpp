#include "main.h"

BSDATA(playeri) = {
	{"Neutral"},
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