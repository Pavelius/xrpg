#include "main.h"

BSDATA(populationi) = {
	{"CursedLands", -200, -2},
	{"DeadLands", 0, -1},
	{"NoPopulated", 30, 0},
	{"SmallHamlet", 100, 1},
	{"SmallVillage", 500, 2},
	{"Village", 1000, 3},
	{"Town", 3000, 4},
	{"City", 5000, 5},
	{"GreatCity", 10000, 6},
};
BSDATAF(populationi)

const populationi* populationi::findbypopulation(int value) {
	for(auto& e : bsdata<populationi>()) {
		if(e.population >= value)
			return &e;
	}
	return bsdata<populationi>::elements + sizeof(bsdata<populationi>::elements) / sizeof(bsdata<populationi>::elements[0]) - 1;
}

crange populationi::getrange() const {
	if(bsdata<populationi>::elements == this)
		return {-1000, -200};
	return {this[-1].population, population};
}