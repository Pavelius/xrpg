#include "crt.h"
#include "draw.h"
#include "draw_geometry.h"

void draw::bezierseg(int x0, int y0, int x1, int y1, int x2, int y2) {
	int sx = x2 - x1, sy = y2 - y1;
	long xx = x0 - x1, yy = y0 - y1, xy;             /* relative values for checks */
	double dx, dy, err, ed, cur = xx * sy - yy * sx;    /* curvature */
	assert(xx * sx <= 0 && yy * sy <= 0);				/* sign of gradient must not change */
	if(sx * (long)sx + sy * (long)sy > xx * xx + yy * yy) { /* begin with longer part */
		x2 = x0; x0 = sx + x1; y2 = y0; y0 = sy + y1; cur = -cur;     /* swap P0 P2 */
	}
	if(cur != 0) {                                                      /* no straight line */
		xx += sx; xx *= sx = x0 < x2 ? 1 : -1;              /* x step direction */
		yy += sy; yy *= sy = y0 < y2 ? 1 : -1;              /* y step direction */
		xy = 2 * xx * yy; xx *= xx; yy *= yy;             /* differences 2nd degree */
		if(cur * sx * sy < 0) {                              /* negated curvature? */
			xx = -xx; yy = -yy; xy = -xy; cur = -cur;
		}
		dx = 4.0 * sy * (x1 - x0) * cur + xx - xy;                /* differences 1st degree */
		dy = 4.0 * sx * (y0 - y1) * cur + yy - xy;
		xx += xx; yy += yy; err = dx + dy + xy;                   /* error 1st step */
		do {
			cur = imin(dx + xy, -xy - dy);
			ed = imax(dx + xy, -xy - dy);               /* approximate error distance */
			ed += 2 * ed * cur * cur / (4 * ed * ed + cur * cur);
			pixel(x0, y0, (unsigned char)(255 * iabs(err - dx - dy - xy) / ed));          /* plot curve */
			if(x0 == x2 || y0 == y2) break;     /* last pixel -> curve finished */
			x1 = x0; cur = dx - err; y1 = 2 * err + dy < 0;
			if(2 * err + dx > 0) {                                        /* x step */
				if(err - dy < ed) pixel(x0, y0 + sy, (unsigned char)(255 * iabs(err - dy) / ed));
				x0 += sx; dx -= xy; err += dy += yy;
			}
			if(y1) {                                                  /* y step */
				if(cur < ed) pixel(x1 + sx, y0, (unsigned char)(255 * iabs(cur) / ed));
				y0 += sy; dy -= xy; err += dx += xx;
			}
		} while(dy < dx);                  /* gradient negates -> close curves */
	}
	caret.x = x0;
	caret.y = y0;
	line(x2, y2);                  /* plot remaining needle to end */
}

void draw::bezier(int x0, int y0, int x1, int y1, int x2, int y2) {
	int x = x0 - x1, y = y0 - y1;
	double t = x0 - 2 * x1 + x2, r;
	if((long)x * (x2 - x1) > 0) {                        /* horizontal cut at P4? */
		if((long)y * (y2 - y1) > 0)                     /* vertical cut at P6 too? */
			if(iabs((y0 - 2 * y1 + y2) / t * x) > iabs(y)) {               /* which first? */
				x0 = x2; x2 = x + x1; y0 = y2; y2 = y + y1;            /* swap points */
			}                            /* now horizontal cut at P4 comes first */
		t = (x0 - x1) / t;
		r = (1 - t) * ((1 - t) * y0 + 2.0 * t * y1) + t * t * y2;                       /* By(t=P4) */
		t = (x0 * x2 - x1 * x1) * t / (x0 - x1);                       /* gradient dP4/dx=0 */
		x = ifloor(t + 0.5); y = ifloor(r + 0.5);
		r = (y1 - y0) * (t - x0) / (x1 - x0) + y0;                  /* intersect P3 | P0 P1 */
		bezierseg(x0, y0, x, ifloor(r + 0.5), x, y);
		r = (y1 - y2) * (t - x2) / (x1 - x2) + y2;                  /* intersect P4 | P1 P2 */
		x0 = x1 = x; y0 = y; y1 = ifloor(r + 0.5);             /* P0 = P4, P1 = P8 */
	}
	if((long)(y0 - y1) * (y2 - y1) > 0) {                    /* vertical cut at P6? */
		t = y0 - 2 * y1 + y2; t = (y0 - y1) / t;
		r = (1 - t) * ((1 - t) * x0 + 2.0 * t * x1) + t * t * x2;                       /* Bx(t=P6) */
		t = (y0 * y2 - y1 * y1) * t / (y0 - y1);                       /* gradient dP6/dy=0 */
		x = ifloor(r + 0.5); y = ifloor(t + 0.5);
		r = (x1 - x0) * (t - y0) / (y1 - y0) + x0;                  /* intersect P6 | P0 P1 */
		bezierseg(x0, y0, ifloor(r + 0.5), y, x, y);
		r = (x1 - x2) * (t - y2) / (y1 - y2) + x2;                  /* intersect P7 | P1 P2 */
		x0 = x; x1 = ifloor(r + 0.5); y0 = y1 = y;             /* P0 = P6, P1 = P7 */
	}
	bezierseg(x0, y0, x1, y1, x2, y2);                  /* remaining part */
}

void draw::spline(point* original_points, int n) {
	point points[256];
	if(n > (int)(sizeof(points) / sizeof(points[0])))
		n = sizeof(points) / sizeof(points[0]);
	n = n - 1;
	memcpy(points, original_points, sizeof(points[0]) * (n + 1));
	const int M_MAX = 6;
	float mi = 1, m[M_MAX];                    /* diagonal constants of matrix */
	int i, x0, y0, x1, y1, x2 = points[n].x, y2 = points[n].y;
	assert(n > 1);                        /* need at least 3 points P[0]..P[n] */
	points[1].x = x0 = 8 * points[1].x - 2 * points[0].x;                          /* first row of matrix */
	points[1].y = y0 = 8 * points[1].y - 2 * points[0].y;
	for(i = 2; i < n; i++) {                                 /* forward sweep */
		if(i - 2 < M_MAX) m[i - 2] = mi = (float)(1.0 / (6.0 - mi));
		points[i].x = x0 = ifloor(8 * points[i].x - x0 * mi + 0.5);                        /* store yi */
		points[i].y = y0 = ifloor(8 * points[i].y - y0 * mi + 0.5);
	}
	x1 = ifloor((x0 - 2 * x2) / (5.0 - mi) + 0.5);                 /* correction last row */
	y1 = ifloor((y0 - 2 * y2) / (5.0 - mi) + 0.5);
	for(i = n - 2; i > 0; i--) {                           /* back substitution */
		if(i <= M_MAX) mi = m[i - 1];
		x0 = ifloor((points[i].x - x1) * mi + 0.5);                            /* next corner */
		y0 = ifloor((points[i].y - y1) * mi + 0.5);
		bezier((x0 + x1) / 2, (y0 + y1) / 2, x1, y1, x2, y2);
		x2 = (x0 + x1) / 2; x1 = x0;
		y2 = (y0 + y1) / 2; y1 = y0;
	}
	bezier(points[0].x, points[0].y, x1, y1, x2, y2);
}

void draw::rectb(int radius) {
	//line(rc.x1 + radius, rc.y1, rc.x2 - radius, rc.y1);
	//line(rc.x2, rc.y1 + radius, rc.x2, rc.y2 - radius);
	//line(rc.x2 - radius, rc.y2, rc.x1 + radius, rc.y2);
	//line(rc.x1, rc.y2 - radius, rc.x1, rc.y1 + radius);
	//bezierseg(rc.x1 + radius, rc.y1, rc.x1, rc.y1, rc.x1, rc.y1 + radius);
	//bezierseg(rc.x2 - radius, rc.y1, rc.x2, rc.y1, rc.x2, rc.y1 + radius);
	//bezierseg(rc.x1 + radius, rc.y2, rc.x1, rc.y2, rc.x1, rc.y2 - radius);
	//bezierseg(rc.x2 - radius, rc.y2, rc.x2, rc.y2, rc.x2, rc.y2 - radius);
}

static void triangle_bottom(point v1, point v2, point v3) {
	auto invslope1 = float(v2.x - v1.x) / float(v2.y - v1.y);
	auto invslope2 = float(v3.x - v1.x) / float(v3.y - v1.y);
	float curx1 = v1.x;
	float curx2 = v1.x;
	for(int scanlineY = v1.y; scanlineY <= v2.y; scanlineY++) {
		//line((int)curx1, scanlineY, (int)curx2, scanlineY);
		curx1 += invslope1;
		curx2 += invslope2;
	}
}

static void triangle_top(point v1, point v2, point v3) {
	float invslope1 = float(v3.x - v1.x) / float(v3.y - v1.y);
	float invslope2 = float(v3.x - v2.x) / float(v3.y - v2.y);
	float curx1 = v3.x;
	float curx2 = v3.x;
	for(int scanlineY = v3.y; scanlineY > v1.y; scanlineY--) {
		//line((int)curx1, scanlineY, (int)curx2, scanlineY);
		curx1 -= invslope1;
		curx2 -= invslope2;
	}
}

void draw::triangle(point v1, point v2, point v3) {
	if(v2.y < v1.y)
		iswap(v1, v2);
	if(v3.y < v1.y)
		iswap(v1, v3);
	if(v3.y < v2.y)
		iswap(v2, v3);
	if(v2.y == v3.y)
		triangle_bottom(v1, v2, v3);
	else if(v1.y == v2.y)
		triangle_top(v1, v2, v3);
	else {
		point v4 = {(short)(v1.x + ((float)(v2.y - v1.y) / (float)(v3.y - v1.y)) * (v3.x - v1.x)), (short)v2.y};
		triangle_bottom(v1, v2, v4);
		triangle_top(v2, v4, v3);
	}
}