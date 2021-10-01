#include "main.h"
#include "draw.h"
#include "draw_simple.h"
#include "draw_input.h"

using namespace draw;

struct cube {
	double x, y, z;
};

const double sqrt_3 = 1.732050807568877;
const double cos_30 = 0.86602540378;
const int size = 50;
extern bool line_antialiasing;
static const point states_pos[] = {{-16, 16}, {-16, -16}, {16, 16}, {16, -16}};

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

static point cube_to_oddr(cube c) {
	auto col = c.x + (c.z - (((int)c.z) & 1)) / 2;
	auto row = c.z;
	return {(short)col, (short)row};
}

static point flat_hex_to_pixel(point hex) {
	auto x = (short)((size * 3.0 * hex.x) / 2);
	auto y = (short)(size * sqrt_3 * hex.x + 0.5 * (hex.y & 1));
	return {x, y};
}

static point pixel_to_flat_hex(point pixel) {
	auto q = (short)(((2.0 / 3.0) * pixel.x) / size);
	auto r = (short)(((-1.0 / 3.0) * pixel.x + sqrt_3 / 3 * pixel.y) / size);
	return {q, r};
}

static cube axial_to_cube(point pt) {
	return {(double)pt.x, (double)(-pt.x - pt.y), (double)pt.y};
}

static point cube_to_axial(cube c) {
	return {(short)c.x, (short)c.z};
}

static void triangle(point v1, point v2) {
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

static void hexagon(int x, int y, int size) {
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

static void hexagonf(short x, short y, int size, unsigned char alpha = 255) {
	auto p1 = point{(short)(x - size * cos_30), (short)(y - size / 2)}; //points[4];
	auto p2 = point{(short)(x + size * cos_30), (short)(y + size / 2)}; //points[1];
	auto push_fore = fore;
	fore.a = alpha;
	rectf({p1.x, p1.y + 1, p2.x + 1, p2.y}, fore, alpha);
	triangle({p2.x, p2.y}, {x, y + size});
	triangle({p2.x, p1.y}, {x, y - size});
	fore = push_fore;
}

static point h2p(point hex) {
	short x = short(size * sqrt_3) * hex.x + (short(size * sqrt_3) / 2) * hex.y;
	short y = size * 3 / 2 * hex.y;
	return {x, y};
}

static point p2h(point pt) {
	auto q = ((sqrt_3 / 3.0) * (double)pt.x - (1.0 / 3.0) * (double)pt.y) / (double)size;
	auto r = ((2.0 / 3.0) * (double)pt.y) / (double)size;
	return cube_to_oddr(cube_round(axial_to_cube({(short)q, (short)r})));
}

static int getdistance(point h1, point h2) {
	auto a = axial_to_cube(h1);
	auto b = axial_to_cube(h2);
	return int(iabs(a.x - b.x) + iabs(a.y - b.y) + iabs(a.z - b.z)) / 2;
}

//static void hexagon(short unsigned i, bool use_hilite, bool show_movement) {
//	auto pt = map::h2p(i) - camera;
//	const rect rc = {0 - 100, 0 - 100, draw::getwidth() + 100, draw::getheight() + 100};
//	if(!pt.in(rc))
//		return;
//	hexagon(pt, hexagon_offset, colors::black);
//	if(map::is(i, HasBlock))
//		hexagon(pt, hexagon_offset2, colors::green);
//	auto m = getmovecost(i);
//	if(m == Blocked)
//		use_hilite = false;
//	if(show_movement && use_hilite && m) {
//		auto pf = fore;
//		fore = colors::black;
//		fore.a = 128;
//		hexagonf(pt.x, pt.y);
//		fore = pf;
//	} else if(show_map_index) {
//		if(m != Blocked) {
//			char temp[32]; stringbuilder sb(temp);
//			sb.add("%1i", i);
//			text(pt.x - textw(temp) / 2, pt.y - texth() / 2, temp);
//		}
//	}
//	if(use_hilite) {
//		auto m = getmovecost(i);
//		if(m != Blocked) {
//			const rect rch = {pt.x - size / 2, pt.y - size / 2, pt.x + size / 2, pt.y + size / 2};
//			if(areb(rch))
//				hilite_index = i;
//		}
//	}
//}

static void paint_number(int x, int y, int value) {
	char temp[16]; stringbuilder sb(temp);
	sb.add("%1i", value);
	auto push_stroke = fore_stroke;
	auto push_font = font;
	fore_stroke = colors::red;
	font = metrics::h2;
	text(x - textw(temp) / 2, y, temp, -1, TextStroke);
	fore_stroke = push_stroke;
	font = push_font;
}

void object::paint() const {
	auto x = caret.x, y = caret.y;
	auto ps = gres(kind.getid(), "art/creatures");
	if(!ps)
		return;
	auto pc = gres("conditions", "art/objects");
	if(!pc)
		return;
	unsigned char alpha = 255;
	if(is(Invisibility))
		alpha = 64;
	image(x, y, ps, 0, 0, alpha);
	auto spi = 0;
	for(auto i = Disarmed; i <= Strenght; i = (state_s)(i + 1)) {
		if(i == Invisibility)
			continue;
		if(is(i)) {
			image(
				x + states_pos[spi].x,
				y + states_pos[spi].y,
				pc, i, 0);
			spi++;
		}
	}
	auto push_fore = fore;
	fore = colors::blue;
	switch(kind.type) {
	case Player: fore = colors::red; break;
	case Monster: fore = colors::white; break;
    default: break;
	}
	hexagon(x, y, size);
	fore = push_fore;
	paint_number(x, y + 30, hits);
}

static bool ishilitehex(int x, int y) {
    auto r = size*2/3;
    rect rc = {x-r, y-r, x+r, y+r};
	return ishilite(rc);
}

static point mouse_difference;

void paintfigures() {
	for(auto& e : bsdata<object>()) {
        if(dragactive(&e))
            caret = hot.mouse - mouse_difference;
        else {
            caret = e.getposition() - camera;
            if(ishilitehex(caret.x, caret.y)) {
                if(hot.key==MouseLeft && hot.pressed) {
                    dragbegin(&e);
                    mouse_difference = hot.mouse - caret;
                }
                scene.hilite = &e;
                draw::tooltips(e.kind.getname());
            }
        }
		e.paint();
//		if(scene.hilite==variant(&e))
//            hexagonf(caret.x, caret.y, size, 64);
	}
}

static void paintgame() {
	paintclear();
	paintimage();
	paintfigures();
	paintcommands();
}

static variant choose_cards(variant player, int level) {
	varianta collection;
	for(auto& e : bsdata<cardi>()) {
		if(e.owner != player)
			continue;
		if(e.level != level)
			continue;
		collection.add(&e);
	}
	return collection.choose(player.getname(), getnm("Cancel"), true, 0);
}

void start_menu() {
	auto pp1 = (playeri*)bsdata<playeri>::source.ptr(0);
	auto pp2 = (playeri*)bsdata<playeri>::source.ptr(1);
	pp1->buildcombatdeck();
	pp2->buildcombatdeck();
	game.buildcombatdeck();
	auto p1 = game.create("Brute", Ally);
	auto p2 = game.create("Thinkerer", Enemy);
	p1->setposition({100, 100});
	p1->set(Poison, 1);
	p2->setposition({200, 200});
	//scripti sc = {};
	//p1->attack(1, 0, 0, 0, {});
	//p1->act("Test string %1i and %2i", 10, 12);
	choose_cards("Brute", 1);
	menui::choose("Start", 0, 0);
}

void draw::initializex() {
	initialize("GH simulator");
	pausetime = 1000;
	scene.resurl = "gloomhaven";
	scene.background = paintgame;
	setnext(start_menu);
	start();
}