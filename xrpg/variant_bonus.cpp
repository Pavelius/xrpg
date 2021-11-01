#include "variant.h"

variant bonusi::getaction(variant v) {
	if(bsdata<varianti>::elements[v.type].source->data == bsdata<bonusi>::elements)
		return bsdata<bonusi>::elements[v.value].type;
	return v;
}

int bonusi::getbonus(variant v) {
	if(bsdata<varianti>::elements[v.type].source->data == bsdata<bonusi>::elements)
		return bsdata<bonusi>::elements[v.value].bonus;
	return 1;
}