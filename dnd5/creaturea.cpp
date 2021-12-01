#include "main.h"

void creaturea::select() {
	auto pb = begin();
	auto pe = endof();
	for(auto& e : bsdata<creature>()) {
		if(!e)
			continue;
		if(pb < pe)
			*pb++ = &e;
	}
	count = pb - begin();
}