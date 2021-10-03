#include "main.h"

void scenarioi::prepare(int stage) {
	for(auto i = 0; i < hms * hms; i++)
		game.setwall(i);
	for(auto& e : tiles) {
		if(!e)
			continue;
		game.create(e.position, e.tile, e.inverse);
	}
}