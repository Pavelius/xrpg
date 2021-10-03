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
static indext current_index;
static point mouse_difference;

point h2p(point v) {
	return draw::h2p(v, size);
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
	fore = push_fore;
	paint_number(x, y + 30, hits);
}

void object::paint_tile() const {
	auto& et = bsdata<tilei>::get(kind.value);
	auto ps = gres(et.id, "art/tiles");
	if(!ps)
		return;
	auto flags = 0;
	if(is(Muddle))
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
	}
}

static bool ishilitehex(int x, int y) {
	auto r = size * 2 / 3;
	rect rc = {x - r, y - r, x + r, y + r};
	return ishilite(rc);
}

static void painthexgrid() {
	current_index = Blocked;
	for(short y = 0; y < hms; y++) {
		for(short x = 0; x < hms; x++) {
			auto ix = h2i({x, y});
			if(game.iswall(ix))
				continue;
			auto pt = h2p({x, y}, size) - camera;
			if(show_hex_grid)
				hexagon(pt.x, pt.y, size);
			if(show_hex_coor) {
				char temp[64]; stringbuilder sb(temp);
				sb.add("%1i, %2i", x, y);
				text(pt.x - textw(temp) / 2, pt.y, temp);
			}
			if(ishilitehex(pt.x, pt.y))
				current_index = ix;
		}
	}
	if(hot.key == (Ctrl + 'G'))
		execute(cbsetint, show_hex_grid ? 0 : 1, 0, &show_hex_grid);
	else if(hot.key == 'G')
		execute(cbsetint, show_hex_coor ? 0 : 1, 0, &show_hex_coor);
}

static void painthilitehex() {
	if(current_index == Blocked)
		return;
	auto pt = h2p(i2h(current_index), size) - camera;
	auto push_fore = fore;
	fore = colors::white;
	fore.a = 64;
	hexagonf(pt.x, pt.y, size);
	fore = push_fore;
}

void paintfigures() {
	for(auto& e : bsdata<object>()) {
		if(dragactive(&e))
			e.setposition(hot.mouse - mouse_difference + camera);
		caret = e.getposition() - camera;
		if(ishilitehex(caret.x, caret.y)) {
			switch(e.kind.type) {
			case Player:
			case SummonedCreature:
			case Monster:
				if(hot.key == MouseLeft && hot.pressed) {
					dragbegin(&e);
					mouse_difference = hot.mouse - caret;
				}
				scene.hilite = &e;
				draw::tooltips(e.kind.getname());
				break;
			}
		}
		e.paint();
		// if(scene.hilite==variant(&e))
		//	hexagonf(caret.x, caret.y, size, 64);
	}
}

static void paintcurrentindex() {
	if(current_index == Blocked)
		return;
	auto mp = i2h(current_index);
	tooltips("Index %1i, %2i", mp.x, mp.y);
}

static void paintgame() {
	paintclear();
	//rectf({0, 0, getwidth(), getheight()}, colors::white);
	paintfigures();
	painthexgrid();
	painthilitehex();
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

static void test_scenario() {
	auto& sc = bsdata<scenarioi>::get(0);
	sc.prepare(0);
	auto p1 = game.create("Brute", Ally);
	auto p2 = game.create("Thinkerer", Enemy);
	p1->setposition(h2p(sc.getstart(0), size));
	p1->set(Poison, 1);
	p2->setposition(h2p(sc.getstart(1), size));
}

void start_menu() {
	auto pp1 = (playeri*)bsdata<playeri>::source.ptr(0);
	auto pp2 = (playeri*)bsdata<playeri>::source.ptr(1);
	pp1->buildcombatdeck();
	pp2->buildcombatdeck();
	game.buildcombatdeck();
	test_scenario();
	//scripti sc = {};
	//p1->attack(1, 0, 0, 0, {});
	//p1->act("Test string %1i and %2i", 10, 12);
	choose_cards("Brute", 1);
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