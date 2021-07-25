#include "main.h"

void variant::getinfo(stringbuilder& sb) const {
	sb.add("##%1\n", getname());
	auto pd = getdescription();
	if(pd)
		sb.addn(pd);
}