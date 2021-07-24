#include "main.h"

gamei	game;

static void field(int x, int& y, int dx, color_s border, color_s color, int value, int maximum) {
	const int dy = 4;
	if(value == maximum)
		return;
	draw::bar({x, y, x + dx, y + dy}, color, border, ColorBlack, value, maximum);
	y += dy;
}

point gamei::getp(indext v) {
	return {
		(short)(worldmap::getx(v) * gui.grid + gui.grid / 2),
		(short)(worldmap::gety(v) * gui.grid + gui.grid / 2)
	};
}

void gamei::render_world() {
	game.world.paint();
	if(game.party.ismoving()) {
		auto pt = game.party.getdestination();
		draw::paint(pt.x, pt.y, FigureClose, ColorRed, 3);
	}
	auto pt = game.party.getposition();
	auto border = ColorGreen;
	rect rc;
	draw::avatar(pt.x, pt.y, "party", border, &rc);
	if(rc) {
		auto x = rc.x1, y = rc.y1, dx = rc.width();
		field(x, y, dx, border, ColorRed, 1, 4);
		field(x, y, dx, border, ColorLightGreen, 4, 4);
		field(x, y, dx, border, ColorBlue, 3, 3);
		field(x, y, dx, border, ColorLightBlue, 3, 4);
	}
}

void gamei::update_moving() {
	auto tick = game.rounds;
	game.party.moving(gui.grid * 4, game.rounds, game.rounds_per_day);
	game.setexplored(game.geti(game.party.getposition()), 1);
	game.rounds++;
}

void gamei::click_order() {
	auto pt = draw::gethiliteback();
	if(pt.x < 0 && draw::hilite_object) {
		locationi* p1 = draw::hilite_object;
		if(p1)
			pt = p1->getposition();
	}
	if(pt.x < 0)
		return;
	game.setmovement(getp(geti(pt)));
}

void gamei::playworld() {
	//struct buyform : draw::list {
	//	const char* getvalue(const char* id, stringbuilder& sb) const override {
	//		if(equal(id, "text"))
	//			return "Покупка предметов и артефактов";
	//		return 0;
	//	}
	//	static void apply() {}
	//	static void cancel() {}
	//};
	gui.bitmap = "map";
	gui.background = render_world;
	draw::scene(update_moving, click_order);
	//buyform list;
	//list.choose({100, 100, 800 - 100, 600 - 100});
}

void gamei::setexplored(indext i, int r) {
	world.set(i, r, 1, true);
	world.set(i, r - 1, 2, false);
}
