#include "main.h"
#include "pathfind.h"

using namespace map;

short unsigned costmap[mpx * mpy];

static indext m2i(point p) {
	return p.y * mpx + p.x;
}

static void clearallcost() {
	for(auto& v : costmap)
		v = CostPassable;
}

void creature::lookmove() {
	clearallcost();
	auto i = m2i(draw::s2m(getposition()));
	wave(costmap, i);
}