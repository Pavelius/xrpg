#pragma once

namespace map {
enum direction_s : unsigned char {
	Up, LeftUp, Left, LeftDown, Down, RightDown, Right, RightUp,
};
typedef short unsigned indext;
const indext	Blocked = 0xFFFF;
const indext	CostPassable = 0xFF00;
const indext	CostInpassable = 0xFFF0;
const indext	mpx = 30;
const indext	mpy = 30;
extern short unsigned costmap[mpx * mpy];
typedef indext(*fnroute)(indext i);
inline indext	gi(int x, int y) { return y * mpx + x; }
inline int		gx(indext i) { return i % mpx; }
inline int		gy(indext i) { return i / mpx; }
indext			moveaway(indext start);
indext			movecloser(indext start);
unsigned		route(indext* destiation, unsigned count, indext start, fnroute proc);
indext			to(indext i, direction_s d);
direction_s		to(direction_s d, direction_s d1);
void			wave(indext start);
}
