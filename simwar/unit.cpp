#include "main.h"

int uniti::get(variant v) const {
	return troop::getbonus(v, stats);
}