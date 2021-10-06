#include "main.h"
#include "draw_hexagon.h"

static short unsigned stack[256 * 256];
static short unsigned movement_rate[hms * hms];

short unsigned mapi::getmove(indext i) const {
	return movement_rate[i];
}

void mapi::getmove(short unsigned* destination) {
	memcpy(destination, movement_rate, sizeof(movement_rate));
}

void mapi::setmove(const short unsigned* source) {
	memcpy(movement_rate, source, sizeof(movement_rate));
}

void mapi::setmove(indext i, short unsigned v) {
	movement_rate[i] = v;
}

void mapi::clearpath() {
	memset(movement_rate, 0, sizeof(movement_rate));
}

indext mapi::getfarest(const short unsigned* source) {
	auto nd = 0;
	auto sd = 40000;
	auto ni = Blocked;
	for(auto i = 0; i < hms * hms; i++) {
		if(source[i] == Blocked || movement_rate[i] == Blocked)
			continue;
		if(source[i] <= sd && movement_rate[i] >= nd) {
			sd = source[i];
			nd = movement_rate[i];
			ni = i;
		}
	}
	return ni;
}

indext mapi::getnearest(const short unsigned* source) {
	auto nd = 40000;
	auto sd = 40000;
	auto ni = Blocked;
	for(auto i = 0; i < hms * hms; i++) {
		if(source[i] == Blocked || movement_rate[i] == Blocked)
			continue;
		if(source[i] <= sd && movement_rate[i] <= nd) {
			sd = source[i];
			nd = movement_rate[i];
			ni = i;
		}
	}
	return ni;
}

void mapi::blockwalls() {
	for(indext i = 0; i < hms * hms; i++) {
		if(iswall(i))
			movement_rate[i] = Blocked;
	}
}

indext mapi::to(indext i, int d) const {
	if(i == Blocked)
		return Blocked;
	auto hex = draw::getdirection(i2h(i), d);
	if(hex.x < 0 || hex.y < 0 || hex.x >= hms || hex.y >= hms)
		return Blocked;
	return h2i(hex);
}

static void blockzero() {
	for(indext i = 0; i < hms * hms; i++) {
		if(!movement_rate[i])
			movement_rate[i] = Blocked;
	}
}

void mapi::makewave(indext start_index) {
	auto stack_end = stack + sizeof(stack) / sizeof(stack[0]);
	auto push_counter = stack;
	auto pop_counter = stack;
	movement_rate[start_index] = Blocked;
	*push_counter++ = start_index;
	while(pop_counter != push_counter) {
		auto index = *pop_counter++;
		if(pop_counter >= stack_end)
			pop_counter = stack;
		auto cost = ((index == start_index) ? 0 : movement_rate[index]) + 1;
		for(int d = 0; d < 6; d++) {
			auto i1 = to(index, d);
			if(i1 == Blocked || movement_rate[i1] == Blocked)
				continue;
			if(movement_rate[i1] && movement_rate[i1] < cost)
				continue;
			movement_rate[i1] = cost;
			*push_counter++ = i1;
			if(push_counter >= stack_end)
				push_counter = stack;
		}
	}
	blockzero();
}

void mapi::block(const objects& source) {
	for(auto v : source) {
		object* p = v;
		if(!p)
			continue;
		setmove(p->getindex(), Blocked);
	}
}

void mapi::blockrange(int range) {
	for(indext i = 0; i < hms * hms; i++) {
		auto v = getmove(i);
		if(v == Blocked)
			continue;
		if(v > range)
			setmove(i, Blocked);
	}
}