#include "main.h"

creature* item::getowner() const {
	auto i = bsdata<creature>::source.indexof(this);
	if(i == -1)
		return 0;
	return (creature*)bsdata<creature>::source.ptr(i);
}