#include "main.h"
#include "draw.h"
#include "draw_hexagon.h"
#include "draw_simple.h"
#include "draw_input.h"

using namespace draw;

const int size = 50;
static const point states_pos[] = {{-16, 16}, {-16, -16}, {16, 16}, {16, -16}};
static int show_hex_coor = 0;
static int show_hex_grid = 0;
int show_movement_cost = 0;
static indext current_index;
static point mouse_difference;
static const object* focused_object;

point h2p(point v) {
	return draw::h2p(v, size);
}

point p2h(point v) {
	return draw::p2h(v, size);
}

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

void object::paint_creature() const {
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
	if(focused_object == this) {
		fore = colors::green;
		hexagon(x, y, size - 2);
	}
	fore = push_fore;
	paint_number(x, y + 30, hits);
}

void object::paint_tile() const {
	auto& et = bsdata<tilei>::get(kind.value);
	auto ps = gres(et.id, "art/tiles");
	if(!ps)
		return;
	auto flags = 0;
	if(is(Mirrored))
		flags = ImageMirrorH | ImageMirrorV;
	image(caret.x, caret.y, ps, 0, flags);
}

void object::paint_default() const {
	auto id = kind.getid();
	auto ps = gres(id, "art/tiles");
	if(!ps)
		return;
	auto flags = 0;
	if(is(Mirrored))
		flags = ImageMirrorH | ImageMirrorV;
	image(caret.x, caret.y, ps, 0, flags);
}

void object::paint() const {
	switch(kind.type) {
	case Player:
	case SummonedCreature:
	case Monster:
		paint_creature();
		break;
	case Tile:
		paint_tile();
		break;
	default:
		paint_default();
		break;
	}
}

static bool ishilitehex(int x, int y) {
	auto r = size * 2 / 3;
	rect rc = {x - r, y - r, x + r, y + r};
	return ishilite(rc);
}

static void paint_tips(point pt, point v) {
	char temp[64]; stringbuilder sb(temp);
	sb.add("%1i, %2i", v.x, v.y);
	text(pt.x - textw(temp) / 2, pt.y, temp);
}

static void paint_tips(point pt, int v) {
	char temp[64]; stringbuilder sb(temp);
	sb.add("%1i", v);
	text(pt.x - textw(temp) / 2, pt.y, temp);
}

static void painthexgrid() {
	current_index = Blocked;
	for(short ix = 0; ix < hms * hms; ix++) {
		auto hex = i2h(ix);
		if(game.iswall(ix))
			continue;
		auto pt = h2p(hex, size) - camera;
		if(show_hex_grid)
			hexagon(pt.x, pt.y, size);
		if(show_movement_cost) {
			auto n = game.getmove(ix);
			if(n != Blocked && n) {
				auto hex = i2h(ix);
				auto pt = h2p(hex, size) - camera;
				paint_tips(pt, n);
				if(ishilitehex(pt.x, pt.y))
					current_index = ix;
			}
		} else if(show_hex_coor)
			paint_tips(pt, hex);
	}
	if(hot.key == (Ctrl + 'G'))
		execute(cbsetint, show_hex_grid ? 0 : 1, 0, &show_hex_grid);
	else if(hot.key == 'G')
		execute(cbsetint, show_hex_coor ? 0 : 1, 0, &show_hex_coor);
	else if(hot.key == 'M')
		execute(cbsetint, show_movement_cost ? 0 : 1, 0, &show_movement_cost);
}

static void painthilitehex() {
	if(!show_movement_cost)
		return;
	if(current_index == Blocked)
		return;
	auto pt = h2p(i2h(current_index), size) - camera;
	auto push_fore = fore;
	fore = colors::white;
	fore.a = 64;
	hexagonf(pt.x, pt.y, size);
	fore = push_fore;
}

void objects::paint(bool allow_hilite, bool allow_drag) const {
    for(auto p : *this) {
        if(dragactive(p))
            p->setposition(hot.mouse - mouse_difference + camera);
        caret = p->getposition() - camera;
        auto hilited = false;
        if(allow_hilite) {
            if(p->kind.type!=Tile)
                hilited = ishilitehex(caret.x, caret.y);
        }
        if(allow_drag && hot.key == MouseLeft && hot.pressed) {
            dragbegin(p);
            mouse_difference = hot.mouse - caret;
        }
        if(hilited) {
            scene.hilite = p;
            auto hex = p2h(p->getposition());
            char temp[260]; stringbuilder sb(temp); sb.add("%1i, %2i", hex.x, hex.y);
            draw::tooltips(temp);
        }
        p->paint();
    }
}

static void paintfigures() {
	objects source;
	source.clear();
	source.select();
	source.sort();
	source.paint(true, false);
}

static void paintgame() {
	paintclear();
	//rectf({0, 0, getwidth(), getheight()}, colors::white);
	paintfigures();
	painthexgrid();
	painthilitehex();
	paintcommands();
}

static object* create_creature(variant v, point hex, bool hostile, int level = 1) {
	auto p = game.create(v, false);
	p->setposition(h2p(hex, size));
	p->set(Level, level);
	p->refresh();
	if(hostile)
		p->set(Hostile, 1);
	return p;
}

static void test_scenario() {
	auto& sc = bsdata<scenarioi>::get(0);
	sc.prepare(0);
	auto p1 = create_creature("Brute", sc.getstart(0), false);
	create_creature("Thinkerer", sc.getstart(1), false);
	create_creature("BanditGuard", {6,10}, true, 0);
	create_creature("BanditGuard", {6, 11}, true, 0);
	create_creature("BanditGuard", {6, 12}, true, 0);
	p1->set(Poison, 1);
	p1->focusing();
	p1->move(3);
	p1->attack(3, 0, 0, 2, {});
	//p1->action(Push, true, true, 4, 1, 2);
	//p2->focusing();
	//p2->move(5);
}

static void choose_movement_window() {
	if(current_index == Blocked)
		return;
	switch(hot.key) {
	case MouseLeft:
		if(hot.pressed)
			execute(breakparam, current_index);
		break;
	}
}

bool object::isfocused() const {
	return focused_object == this;
}

void object::focusing() const {
	focused_object = this;
	draw::centercamera(getposition());
}

indext draw::choosemovement() {
	auto push_window = scene.window;
	auto push_show = show_movement_cost;
	show_movement_cost = 1;
	scene.window = choose_movement_window;
	simpleui();
	show_movement_cost = push_show;
	scene.window = push_window;
	return (indext)hot.param;
}

void draw::centercamera(point v) {
	v.x -= getwidth() / 2;
	v.y -= getheight() / 2;
	draw::moving(camera, v);
}

bool test_convert_all() {
    for(auto y = 0; y<hms; y++) {
        for(auto x = 0; x<hms; x++) {
            point hex = {(short)x, (short)y};
            point scr = h2p(hex, size);
            point he1 = p2h(scr, size);
            if(hex!=he1)
                return false;
        }
    }
    return true;
}

void start_menu() {
	auto pp1 = (playeri*)bsdata<playeri>::source.ptr(0);
	auto pp2 = (playeri*)bsdata<playeri>::source.ptr(1);
	pp1->buildcombatdeck();
	pp2->buildcombatdeck();
	game.buildcombatdeck();
	if(!test_convert_all())
        return;
	test_scenario();
	//scripti sc = {};
	//p1->attack(1, 0, 0, 0, {});
	//p1->act("Test string %1i and %2i", 10, 12);
	//choose_cards("Brute", 1);
	menui::choose("Start", 0, 0);
}

void draw::initializex() {
	initialize("GH simulator");
	pausetime = 1000;
	scene.resurl = "L1a";
	scene.background = paintgame;
	setnext(start_menu);
	start();
}