#include "main.h"

int getfix(stringbuilder* sb, int v, variant id) {
	if(v == 0 || !sb)
		return 0;
	auto name = id.getname();
	if(name && name[0]) {
		if(v > 0)
			sb->addn("%1i\t%2", v, name);
		else
			sb->addn("-%1i\t%2", -v, name);
	}
	return v;
}