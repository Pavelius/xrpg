#include "main.h"

static int last_roll, last_roll_twin;

static int roll20(bool lucky) {
	auto r = 1 + rand() % 20;
	if(r == 1 && lucky)
		r = 1 + rand() % 20;
	return r;
}

int statable::roll(int advantage, bool lucky) {
	last_roll = roll20(lucky);
	last_roll_twin = 0;
	if(advantage > 0) {
		last_roll_twin = roll20(lucky);
		if(last_roll_twin < last_roll)
			iswap(last_roll, last_roll_twin);
	} else if(advantage < 0) {
		last_roll_twin = roll20(lucky);
		if(last_roll_twin > last_roll)
			iswap(last_roll, last_roll_twin);
	}
	return last_roll;
}

void statable::copy(statable& dest, const statable& source) {
	dest = source;
}