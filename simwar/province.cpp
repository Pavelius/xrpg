#include "main.h"

static uniti* random(uniti** source4, uniti* def) {
	if(!source4[0])
		return def;
	auto count = 1;
	while(count < 4 && source4[count])
		count++;
	return source4[rand() % count];
}

void provincei::initialize() {
	selector source;
	source.select(bsdata<uniti>::source);
	source.match(landscape, true);
	source.shuffle();
	if(!source)
		return;
	uniti* p = source.data[0];
	if(!p)
		return;
	for(auto count = xrand(1, 4); count > 0; count--) {
		if(chance(75) || count==1)
			game.addtroop(p, this);
		else if(chance(75))
			game.addtroop(random(p->encounter_tought, p), this);
		else
			game.addtroop(random(p->encounter_monster, p), this);
	}
}