#include "crt.h"
#include "pathfind.h"

using namespace map;

static indext path_stack[256 * 256];
static unsigned short path_push;
static unsigned short path_pop;

static const direction_s rotate_direction[4][4] = {
	{Down, Left, Up, Right},
	{Left, Up, Right, Down},
	{Up, Right, Down, Left},
	{Right, Down, Left, Up},
};

direction_s map::to(direction_s d, direction_s d1) {
	return rotate_direction[d - Left][d1 - Left];
}

indext map::to(indext index, direction_s d) {
	if(index == Blocked)
		return Blocked;
	switch(d) {
	case Left:
		if(gx(index) == 0)
			return Blocked;
		return index - 1;
	case Right:
		if(gx(index) >= mpx - 1)
			return Blocked;
		return index + 1;
	case Up:
		if(gy(index) == 0)
			return Blocked;
		return index - mpx;
	case Down:
		if(gy(index) >= mpy - 1)
			return Blocked;
		return index + mpx;
	default:
		return Blocked;
	}
}

static void snode(indext index, short unsigned* pathmap, short unsigned cost) {
	if(index == Blocked)
		return;
	auto a = pathmap[index];
	if(a >= CostInpassable)
		return;
	if(a <= CostPassable) {
		if(cost >= a)
			return;
	} else
		cost += a - CostPassable; // Variable cost for movement
	path_stack[path_push++] = index;
	pathmap[index] = cost;
}

void map::wave(short unsigned* pathmap, indext start) {
	if(start == Blocked || !pathmap)
		return;
	path_push = path_pop = 0;
	path_stack[path_push++] = start;
	pathmap[start] = 1;
	while(path_push != path_pop) {
		auto pos = path_stack[path_pop++];
		auto cost = pathmap[pos];
		snode(to(pos, Left), pathmap, cost + 1);
		snode(to(pos, Right), pathmap, cost + 1);
		snode(to(pos, Up), pathmap, cost + 1);
		snode(to(pos, Down), pathmap, cost + 1);
	}
}