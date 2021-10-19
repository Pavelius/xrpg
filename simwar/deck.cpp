#include "main.h"

void decki::insert(int index, variant v) {
	auto count_before = getcount(); add();
	memmove(data + (index + 1), data + index, (count_before - index) * sizeof(data[0]));
	data[index] = v;
}

void decki::addrandom(variant v) {
	auto n = getcount();
	if(n > 8)
		insert(xrand(8, n - 1), v);
	else
		add(v);
}