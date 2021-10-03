#include "crt.h"
#include "draw.h"
#include "draw_hexagon.h"

using namespace draw;

const double sqrt_3 = 1.732050807568877;
const double cos_30 = 0.86602540378;
extern bool line_antialiasing;

namespace {
struct cube {
	double x, y, z;
};
}

static void triangle2(point v1, point v2) {
	float invslope1 = (float)(v2.x - v1.x) / (float)(v2.y - v1.y);
	float curx1 = v1.x;
	float curx2 = float(v1.x + (v2.x - v1.x) * 2);
	auto push_line = line_antialiasing;
	line_antialiasing = false;
	if(v1.y < v2.y) {
		for(auto scanlineY = v1.y; scanlineY <= v2.y; scanlineY++) {
			line((int)curx1, scanlineY, (int)curx2, scanlineY);
			curx1 += invslope1;
			curx2 -= invslope1;
		}
	} else {
		for(auto scanlineY = v1.y; scanlineY >= v2.y; scanlineY--) {
			line((int)curx1, scanlineY, (int)curx2, scanlineY);
			curx1 -= invslope1;
			curx2 += invslope1;
		}
	}
	line_antialiasing = push_line;
}

static cube cube_round(cube c) {
	double rx = int(c.x);
	double ry = int(c.y);
	double rz = int(c.z);
	auto x_diff = iabs((double)rx - c.x);
	auto y_diff = iabs((double)ry - c.y);
	auto z_diff = iabs((double)rz - c.z);
	if(x_diff > y_diff && x_diff > z_diff)
		rx = -ry - rz;
	else if(y_diff > z_diff)
		ry = -rx - rz;
	else
		rz = -rx - ry;
	return {rx, ry, rz};
}

static point cube_to_evenq(cube c) {
	auto col = c.x;
	auto row = c.z + (c.x + ((int)c.x & 1)) / 2;
	return {(short)col, (short)row};
}

static point cube_to_evenr(cube c) {
	auto col = c.x + (c.z + ((int)c.z & 1)) / 2;
	auto row = c.z;
	return {(short)col, (short)row};
}

static point cube_to_oddr(cube c) {
	auto col = c.x + (c.z - (((int)c.z) & 1)) / 2;
	auto row = c.z;
	return {(short)col, (short)row};
}

static cube axial_to_cube(point pt) {
	return {(double)pt.x, (double)(-pt.x - pt.y), (double)pt.y};
}

static point cube_to_axial(cube c) {
	return {(short)c.x, (short)c.z};
}

point draw::h2p(point hex, int size) {
	auto x = (short)(size * sqrt_3 * (hex.x - 0.5 * (hex.y & 1)));
	auto y = (short)(size * 3 / 2 * hex.y);
	return {x, y};
}

point draw::p2h(point pt, int size) {
	auto q = ((sqrt_3 / 3.0) * (double)pt.x - (1.0 / 3.0) * (double)pt.y) / (double)size;
	auto r = ((2.0 / 3.0) * (double)pt.y) / (double)size;
	return cube_to_evenr(cube_round({q, -q - r, r}));
}

int draw::getdistance(point h1, point h2) {
	auto a = axial_to_cube(h1);
	auto b = axial_to_cube(h2);
	return int(iabs(a.x - b.x) + iabs(a.y - b.y) + iabs(a.z - b.z)) / 2;
}

void draw::hexagon(int x, int y, int size) {
	point points[6] = {{(short)(x + size * cos_30), (short)(y - size / 2)},
		{(short)(x + size * cos_30), (short)(y + size / 2)},
		{(short)x, (short)(y + size)},
		{(short)(x - size * cos_30), (short)(y + size / 2)},
		{(short)(x - size * cos_30), (short)(y - size / 2)},
		{(short)x, (short)(y - size)},
	};
	for(auto i = 0; i < 5; i++) {
		auto p1 = points[i];
		auto p2 = points[i + 1];
		line(p1.x, p1.y, p2.x, p2.y);
	}
	auto p1 = points[5];
	auto p2 = points[0];
	line(p1.x, p1.y, p2.x, p2.y);
}

void draw::hexagonf(short x, short y, int size) {
	auto p1 = point{(short)(x - size * cos_30), (short)(y - size / 2)}; //points[4];
	auto p2 = point{(short)(x + size * cos_30), (short)(y + size / 2)}; //points[1];
	rectf({p1.x, p1.y + 1, p2.x + 1, p2.y}, fore, fore.a);
	triangle2({p2.x, p2.y}, {(short)x, (short)(y + size)});
	triangle2({p2.x, p1.y}, {(short)x, (short)(y - size)});
}

point draw::getdirection(point hex, int direction) {
	static point evenr_directions[2][6] = {
		{{+1, 0}, {1, -1}, {0, -1}, {-1, 0}, {0, +1}, {+1, +1}},
		{{+1, 0}, {0, -1}, {-1, -1}, {-1, 0}, {-1, +1}, {0, +1}},
	};
	auto parity = hex.y & 1;
	auto offset = evenr_directions[parity][direction];
	return hex + offset;
}