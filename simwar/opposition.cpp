#include "main.h"

void oppositioni::recover(int bonus) {
	if(!type)
		return;
	int i = percent;
	int n = 1 + bonus;
	i += xrand(1 * n, 4 * n);
	if(i > 100)
		i = 100;
	percent = i;
}