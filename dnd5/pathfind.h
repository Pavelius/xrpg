#pragma once

namespace map {
enum direction_s : unsigned char {
	Up, Left, Down, Right
};
typedef short unsigned indext;
const indext	Blocked = 0xFFFF;
const indext	CostPassable = 0xFF00;
const indext	CostInpassable = 0xFFF0;
const indext	mpx = 30;
const indext	mpy = 30;
inline indext	gi(int x, int y) { return y * mpx + x; }
inline int		gx(indext i) { return i % mpx; }
inline int		gy(indext i) { return i / mpx; }
indext			to(indext i, direction_s d);
direction_s		to(direction_s d, direction_s d1);
void			wave(short unsigned* cost, indext start);
}
