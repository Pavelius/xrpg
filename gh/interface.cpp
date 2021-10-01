#include "main.h"
#include "draw.h"
#include "draw_simple.h"
#include "draw_input.h"

using namespace draw;

const int size = 50;
static const point states_pos[] = {{-16, 16}, {-16, -16}, {16, 16}, {16, -16}};
static int show_hex_coor = 0;

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
	auto r = size * 2 / 3;
	rect rc = {x - r, y - r, x + r, y + r};
	return ishilite(rc);
}

static point mouse_difference;

static point coor_values;

static void painthexgrid() {
	for(short y = 0; y < 6; y++) {
		for(short x = 0; x < 6; x++) {
			auto pt = h2p({x, y}, size) - camera;
			hexagon(pt.x, pt.y, size);
			if(show_hex_coor) {
				char temp[64]; stringbuilder sb(temp);
				sb.add("%1i, %2i", x, y);
				text(pt.x - textw(temp) / 2, pt.y, temp);
			}
		}
	}
	if(hot.key == 'G')
		execute(cbsetint, show_hex_coor ? 0 : 1, 0, &show_hex_coor);
}

void paintfigures() {
	for(auto& e : bsdata<object>()) {
		if(dragactive(&e))
			e.setposition(hot.mouse - mouse_difference + camera);
		caret = e.getposition() - camera;
		if(ishilitehex(caret.x, caret.y)) {
			if(hot.key == MouseLeft && hot.pressed) {
				dragbegin(&e);
				mouse_difference = hot.mouse - caret;
			}
			scene.hilite = &e;
			draw::tooltips(e.kind.getname());
		}
		e.paint();
		// if(scene.hilite==variant(&e))
		//	hexagonf(caret.x, caret.y, size, 64);
	}
}

static void paintgame() {
	paintclear();
	paintimage();
	painthexgrid();
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
	p1->setposition(h2p({5, 3}, size));
	p1->set(Poison, 1);
	p2->setposition(h2p({4, 6}, size));
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