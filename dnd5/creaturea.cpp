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

void creaturea::select(std::initializer_list<variant> source) {
	auto pb = begin();
	auto pe = endof();
	for(auto& e : bsdata<creature>()) {
		if(!e)
			continue;
		if(!e.ismatch(source, Plus))
			continue;
		if(pb < pe)
			*pb++ = &e;
	}
	count = pb - begin();
}