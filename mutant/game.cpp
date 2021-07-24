#include "main.h"

gamei	game;

void gamei::render_world() {
	game.world.paint();
	if(game.party.ismoving()) {
		auto pt = game.party.getdestination();
		draw::paint(pt.x, pt.y, FigureClose, ColorRed, 3);
	}
	auto pt = game.party.getposition();
	draw::paint(pt.x, pt.y, "Игроки", FigureCircle, ColorGreen, 2);
}

void gamei::update_moving() {
	auto tick = game.rounds;
	game.party.moving(gui.grid * 3, game.rounds, game.rounds_per_day);
	game.setexplored(game.geti(game.party.getposition()), 1);
	for(auto& e : bsdata<locationi>()) {
		if(e)
			e.moving(e.getvelocity(), tick, game.rounds_per_day);
	}
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
	game.setmovement(pt);
}

void gamei::setexplored(indext i, int r) {
	world.set(i, r, 1, true);
	world.set(i, r - 1, 2, false);
}
