#include "point.h"

#pragma once

namespace draw {
void bezier(int x0, int y0, int x1, int y1, int x2, int y2);
void bezierseg(int x0, int y0, int x1, int y1, int x2, int y2);
void rectb(int radius);
void spline(point* points, int n);
void triangle(point v1, point v2, point v3);
}