#include "main.h"

gamei	game;

void gamei::render_world() {
	game.world.paint();
}

void gamei::update_moving() {
	auto tick = game.rounds;
	for(auto& e : bsdata<locationi>()) {
		if(e)
			e.moving(e.getvelocity(), tick, game.rounds_per_day);
	}
}

void gamei::setexplored(indext i, int r) {
	world.set(i, r, 1, true);
	world.set(i, r - 1, 2, false);
}
