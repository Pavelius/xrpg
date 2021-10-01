#include "point.h"

#pragma once

namespace draw {
point					getdirection(point pt, int d);
int						getdistance(point h1, point h2);
point					h2p(point hex, int size);
void					hexagon(int x, int y, int size);
void					hexagonf(short x, short y, int size);
point					p2h(point pt, int size);
}
