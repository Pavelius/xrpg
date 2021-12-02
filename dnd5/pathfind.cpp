#include "crt.h"
#include "pathfind.h"

using namespace map;

short unsigned map::costmap[mpx * mpy];
static indext path_stack[256 * 256];
static unsigned short path_push;
static unsigned short path_pop;

static direction_s all_around[] = {Right, Left, Up, Down};

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

static void snode(indext index, short unsigned cost) {
	if(index == Blocked)
		return;
	auto a = costmap[index];
	if(a >= CostInpassable)
		return;
	if(a <= CostPassable) {
		if(cost >= a)
			return;
	} else
		cost += a - CostPassable; // Variable cost for movement
	path_stack[path_push++] = index;
	costmap[index] = cost;
}

void map::wave(indext start) {
	if(start == Blocked)
		return;
	path_push = path_pop = 0;
	path_stack[path_push++] = start;
	costmap[start] = 1;
	while(path_push != path_pop) {
		auto pos = path_stack[path_pop++];
		auto cost = costmap[pos];
		snode(to(pos, Left), cost + 1);
		snode(to(pos, Right), cost + 1);
		snode(to(pos, Up), cost + 1);
		snode(to(pos, Down), cost + 1);
	}
}

indext map::movecloser(indext start) {
	auto i = Blocked;
	auto a = costmap[start];
	for(auto d : all_around) {
		auto i1 = to(start, d);
		if(i1 == Blocked)
			continue;
		if(costmap[i1] >= a)
			continue;
		i = i1;
		a = costmap[i1];
	}
	return i;
}

indext map::moveaway(indext start) {
	auto i = Blocked;
	auto a = costmap[start];
	for(auto d : all_around) {
		auto i1 = to(start, d);
		if(i1 == Blocked)
			continue;
		if(costmap[i1] <= a)
			continue;
		i = i1;
		a = costmap[i1];
	}
	return i;
}

unsigned map::route(indext* destiation, unsigned count, indext start, fnroute proc) {
	auto pb = destiation;
	auto pe = pb + count;
	auto i = start;
	while(pb < pe) {
		if(i == Blocked)
			break;
		*pb++ = i;
		i = proc(i);
	}
	return pb - destiation;
}