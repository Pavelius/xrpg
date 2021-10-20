#include "main.h"

void provincei::initialize() {
	selector source;
	source.select(bsdata<uniti>::source);
	source.match(landscape, true);
	source.shuffle();
	auto level = 2;
	for(auto v : source)
		game.addtroop(v, this);
}