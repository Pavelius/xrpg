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

playeri* playeri::getcurrent() {
	return bsdata<playeri>::elements + 0;
}

int playeri::get(variant v) const {
	switch(v.type) {
	case Cost: return total.get(v.value);
	default: return 0;
	}
	return 0;
}