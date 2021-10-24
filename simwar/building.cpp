#include "main.h"

void buildingi::apply(stata& stats, costa& cost) const {
	for(auto v : effect)
		game.apply(v, stats, cost);
}