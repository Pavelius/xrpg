#include "main.h"

static uniti* random(uniti** source4, uniti* def) {
	if(!source4[0])
		return def;
	auto count = 1;
	while(count < 4 && source4[count])
		count++;
	return source4[rand() % count];
}

void provincei::generate_units() {
	selector source;
	source.select(bsdata<uniti>::source);
	source.match(landscape, true);
	source.shuffle();
	if(!source)
		return;
	uniti* p = source.data[0];
	if(!p)
		return;
	dwellers = p;
	for(auto count = xrand(1, 4); count > 0; count--) {
		if(chance(75) || count == 1)
			game.addtroop(p, this);
		else if(chance(75))
			game.addtroop(random(p->encounter_tought, p), this);
		else
			game.addtroop(random(p->encounter_monster, p), this);
	}
}

void provincei::generate_explored() {
	stats.set(Explored, landscape->explored.random());
}

void provincei::generate_population() {
	auto value = landscape->population.random();
	stats.set(Population, value);
}

void provincei::initialize() {
	generate_population();
	generate_explored();
	generate_units();
}