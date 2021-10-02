#include "main.h"
#include "draw.h"
#include "draw_simple.h"
#include "draw_input.h"

using namespace draw;

const int size = 50;
static const point states_pos[] = {{-16, 16}, {-16, -16}, {16, 16}, {16, -16}};
static int show_hex_coor = 0;
static int show_hex_grid = 0;
static indext current_index;
static point mouse_difference;

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

static void painthexgrid() {
	current_index = Blocked;
	for(short y = 0; y < 7; y++) {
		for(short x = 0; x < 5; x++) {
			auto pt = h2p({x, y}, size) - camera;
			if(show_hex_grid)
				hexagon(pt.x, pt.y, size);
			if(show_hex_coor) {
				char temp[64]; stringbuilder sb(temp);
				sb.add("%1i, %2i", x, y);
				text(pt.x - textw(temp) / 2, pt.y, temp);
			}
			if(ishilitehex(pt.x, pt.y))
				current_index = h2i({x, y});
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

static void paintcurrentindex() {
	if(current_index == Blocked)
		return;
	auto mp = i2h(current_index);
	tooltips("Index %1i, %2i", mp.x, mp.y);
}

static void paintmap() {
	auto& e = bsdata<tilei>::get(0);
	auto ps = gres(e.id, "art/tiles");
	if(!ps)
		return;
	image(-camera.x, -camera.y, ps, 0, 0);
	painthexgrid();
}

static void paintgame() {
	paintclear();
	paintmap();
	paintfigures();
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

static bool test_tiles() {
	auto& e = bsdata<tilei>::get(0);
	if(!e.size)
		return false;
	if(!e.blocks)
		return false;
	return true;
}

void start_menu() {
	test_tiles();
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
	scene.resurl = "L1a";
	scene.background = paintgame;
	setnext(start_menu);
	start();
}