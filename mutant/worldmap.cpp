#include "draw.h"
#include "main.h"

void worldmap::set(indext i, int r, int v, bool check) {
	auto x1 = getx(i) - r;
	auto x2 = getx(i) + r;
	auto y1 = gety(i) - r;
	auto y2 = gety(i) + r;
	for(auto x = x1; x <= x2; x++) {
		if(x < 0 || x >= xmax)
			continue;
		for(auto y = y1; y <= y2; y++) {
			if(y < 0 || y >= ymax)
				continue;
			auto i0 = geti(x, y);
			if(check && get(i0) != 0)
				continue;
			set(i0, v);
		}
	}
}

point worldmap::getp(int x, int y) {
	return {
		(short)(x * draw::grid_size + draw::grid_size / 2),
		(short)(y * draw::grid_size + draw::grid_size / 2)
	};
}

void worldmap::set(indext i, int v) {
	if(i == Blocked)
		return;
	datamap<xmax*ymax>::set(i, v);
}

void worldmap::explore(indext i) {
	set(i, 1, 1, true);
	set(i, 2);
}

void worldmap::fow() {
	auto push_caret = draw::caret;
	for(auto x = 0; x < xmax; x++) {
		for(auto y = 0; y < ymax; y++) {
			auto v = get(geti(x, y));
			draw::caret.x = x * draw::grid_size;
			draw::caret.y = y * draw::grid_size;
			switch(v) {
			case 0: draw::fog(0xFF); break;
			case 1: draw::fog(128); break;
			default: break;
			}
		}
	}
	draw::caret = push_caret;
}

void worldmap::paint() {
	draw::grid();
	for(auto& e : bsdata<locationi>()) {
		if(e)
			e.paint();
	}
	fow();
}