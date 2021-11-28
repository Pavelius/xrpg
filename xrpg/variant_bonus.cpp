#include "variant.h"

variant variant::getaction() const {
	if(bsdata<varianti>::elements[type].source->data == bsdata<bonusi>::elements)
		return bsdata<bonusi>::elements[value].type;
	return *this;
}

int variant::getbonus() const {
	if(bsdata<varianti>::elements[type].source->data == bsdata<bonusi>::elements)
		return bsdata<bonusi>::elements[value].bonus;
	return 1;
}