#include "main.h"

void provincei::initialize() {
	selector source;
	source.select(bsdata<uniti>::source);
	source.match(landscape, true);
	source.shuffle();
	auto level = xrand(1,3);
	for(auto v : source) {
		game.addtroop(v, this);
		if(--level <= 0)
			break;
	}
}