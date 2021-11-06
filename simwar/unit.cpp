#include "main.h"

int uniti::get(variant v) const {
	switch(v.type) {
	case Stat: return stats.get((stat_s)v.value);
	case Cost: return cost.get(v.value);
	case Tag: return tags.is(v.value) ? 1 : 0;
	default: return 0;
	}
}