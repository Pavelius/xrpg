#include "main.h"

int uniti::get(variant v) const {
	switch(v.type) {
	case Stat: return stats.get(v.value);
	case Cost: return cost.get(v.value);
	default: return 0;
	}
}