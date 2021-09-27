#include "main.h"

void deck::addcombat(variant owner) {
	for(auto& e : bsdata<combatcardi>()) {
		if(e.owner != owner)
			continue;
		for(auto i = 0; i < e.count; i++)
			add(&e);
	}
}

void deck::shuffle() {
	zshuffle(data, count);
}