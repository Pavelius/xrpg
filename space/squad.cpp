#include "main.h"

void squad::create(unsigned char type) {
	this->type = type;
	this->count = geti().count;
	this->wounded = 0;
}

void squad::wound() {
	if(!count)
		return;
	if(wounded < count)
		wounded++;
	else
		kill();
}

void squad::kill() {
	if(!count)
		return;
	if(wounded)
		wounded--;
	count--;
}

bool squad::is(condition_s v) const {
	switch(v) {
	case GoodState: return !wounded;
	case MiddleState: return wounded && wounded < count;
	case BadState: return wounded >= count;
	default: return false;
	}
}

void squad::getinfo(stringbuilder& sb) const {
	auto& ei = geti();
	sb.add(getnm(ei.id));
	sb.adds("(%1i %2)", count, "человек");
}