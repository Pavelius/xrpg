#include "main.h"

void scenarioi::prepare(int stage) {
	for(auto i = 0; i < hms * hms; i++)
		game.setwall(i);
	for(auto& e : tiles) {
		if(!e)
			continue;
		bsdata<tilei>::get(e.tile.value).create(e.position, e.inverse);
	}
}