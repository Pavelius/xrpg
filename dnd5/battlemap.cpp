#include "main.h"

terrain_s map::blocks[mpx * mpy];
adat<indext, 1024> map::indecies;

struct combatactioni {
	const char*			id;
	fnaction			proc;
};

static indext m2i(point p) {
	return p.y * mpx + p.x;
}

bool map::isblocked(indext i) {
	if(i == Blocked)
		return true;
	switch(blocks[i]) {
	case Inpassable: return false;
	default: return true;
	}
}

static void clearallcost() {
	for(auto& v : costmap)
		v = CostPassable;
}

unsigned map::routeto(indext target) {
	indecies.count = route(indecies.data, indecies.getmaximum(), target, movecloser);
	return indecies.count;
}

static void blockinpassable() {
	for(auto i = 0; i < mpx * mpy; i++) {
		if(isblocked(i))
			costmap[i] = Blocked;
	}
}

static void blockmovement(int cost) {
	for(auto i = 0; i < mpx * mpy; i++) {
		if(costmap[i] > cost)
			costmap[i] = Blocked;
	}
}

static void blockcreatures() {
	for(auto& e : bsdata<creature>()) {
		if(!e)
			continue;
		auto i = m2i(draw::s2m(e.getposition()));
		costmap[i] = Blocked;
	}
}

void creature::lookmove() {
	clearallcost();
	blockinpassable();
	blockcreatures();
	wave(m2i(draw::s2m(getposition())));
	blockmovement(get(Speed));
}

BSDATA(combatactioni) = {
	{"Attack", &creature::melee},
};