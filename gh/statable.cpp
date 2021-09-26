#include "main.h"

int statable::get(variant i) const {
	switch(i.type) {
	case Action: return actions[i.value];
	case State: return states.is(i.value) ? 1 : 0;
	default: return 0;
	}
}

void statable::set(variant i, int v) {
	switch(i.type) {
	case Action: actions[i.value] = v; break;
	case State: states.set(i.value); break;
	}
}