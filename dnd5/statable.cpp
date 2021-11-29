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

int statable::roll(dice_s v) {
	return bsdata<dicei>::elements[v].result.roll();
}

void statable::copy(statable& dest, const statable& source) {
	dest = source;
}

static int compare_char(const void* v1, const void* v2) {
	return *((char*)v2) - *((char*)v1);
}

static int find(ability_s* p, int count, ability_s v) {
	for(auto i = 0; i < count; i++) {
		if(p[i] == v)
			return i;
	}
	return -1;
}

void statable::random_ability(classi& kind) {
	static char standart_ability[] = {15, 14, 13, 12, 10, 8};
	char temp[sizeof(standart_ability) / sizeof(standart_ability[0])];
	memcpy(temp, standart_ability, sizeof(temp));
	qsort(temp, 6, sizeof(temp[0]), compare_char);
	ability_s indecies[] = {Strenght, Dexterity, Constitution, Intellect, Wisdow, Charisma};
	zshuffle(indecies, 6);
	iswap(indecies[0], indecies[find(indecies, 6, kind.best)]);
	for(auto v : indecies)
		abilities[v] = temp[v - Strenght];
}