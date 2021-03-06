#include "draw.h"
#include "main.h"

gamei	game;

//static void field(int x, int& y, int dx, color_s border, color_s color, int value, int maximum) {
//	const int dy = 4;
//	if(value == maximum)
//		return;
//	draw::bar({x, y, x + dx, y + dy}, color, border, ColorBlack, value, maximum);
//	y += dy;
//}

point gamei::getp(indext v) {
	return {
		(short)(worldmap::getx(v) * draw::grid_size + draw::grid_size / 2),
		(short)(worldmap::gety(v) * draw::grid_size + draw::grid_size / 2)
	};
}

void gamei::render_world() {
	game.world.paint();
	if(game.party.ismoving()) {
		auto push_caret = draw::caret;
		draw::caret = game.party.getdestination();
		draw::field(figure::Close, 3);
		draw::caret = push_caret;
	}
	auto pt = game.party.getposition();
	//auto border = ColorGreen;
	rect rc;
	//draw::avatar(pt.x, pt.y, "party", border, &rc);
	if(rc) {
		auto x = rc.x1, y = rc.y1, dx = rc.width();
		//field(x, y, dx, border, ColorRed, 1, 4);
		//field(x, y, dx, border, ColorLightGreen, 4, 4);
		//field(x, y, dx, border, ColorBlue, 3, 3);
		//field(x, y, dx, border, ColorLightBlue, 3, 4);
	}
}

void gamei::update_moving() {
	game.party.moving(draw::grid_size * 4, game.rounds, game.rounds_per_day);
	game.setexplored(game.geti(game.party.getposition()), 1);
	game.rounds++;
}

void gamei::worldmap_input() {
	//switch(draw::hot.key) {
	//case InputTimer: update_moving(); break;
	//case MouseLeft:
	//	if(draw::hot.pressed) {
	//		auto pt = draw::gethiliteback();
	//		if(pt.x < 0 && draw::hilite_object) {
	//			locationi* p1 = draw::hilite_object;
	//			if(p1)
	//				pt = p1->world.getposition();
	//		}
	//		if(pt.x < 0)
	//			return;
	//		game.setmovement(getp(geti(pt)));
	//	}
	//	break;
	//}
}

void gamei::playworld() {
}

void gamei::setexplored(indext i, int r) {
	world.set(i, r, 1, true);
	world.set(i, r - 1, 2, false);
}