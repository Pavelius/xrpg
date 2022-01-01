#include "main.h"

void wearable::equip(item& v) {
	for(auto i = Head; i <= Legs; i = (wear_s)(i + 1)) {
		if(!v.geti().is(i))
			continue;
		if(!wears[i])
			continue;
		wears[i] = v;
		v.clear();
		return;
	}
}