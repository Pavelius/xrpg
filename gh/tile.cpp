#include "main.h"

static point block_I1[] = {{0, 1}, {0, 3}};
static point block_L1[] = {{0, 1}, {0, 3}, {0, 5}};

BSDATA(tilei) = {
	{"Corridor", {1, 1}},
	{"G1b", {8, 3}, {-304, -76}},
	{"I1b", {6, 5}, {-217, -151}, block_I1},
	{"L1a", {5, 7}, {-173, -225}, block_L1},
	{"SpikedTrap", {1, 1}},
	{"StoneDoorH", {1, 1}},
	{"StoneDoorV", {1, 1}},
};
BSDATAF(tilei)

static void apply_rect(point position, point size, bool inverse) {
	auto x2 = position.x + size.x;
	auto y2 = position.y + size.y;
	for(short x = position.x; x < x2; x++) {
		for(short y = position.y; y < y2; y++)
			game.setpass(h2i({x, y}));
	}
}

void tilei::creating(point position, bool inverse) const {
	apply_rect(position, size, inverse);
	for(auto pt : blocks) {
		auto i = h2i(pt + position);
		game.setwall(i);
	}
}