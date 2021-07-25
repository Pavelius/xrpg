#include "main.h"

BSDATA(itemi) = {
	{"NoItem"},
	{"BluntInstrument", {}, 1, 1},
	{"BicycleChain", {}, 1, 1},
	{"BrassKnuckles", {Scrap, Light}, 1, 1},
	{"ScrapKnife", {Scrap, Light}, 1, 2},
	{"BaseballBat", {}, 2, 1},
	{"BatWithSpikes", {Scrap}, 2, 2},
	{"Machete", {Scrap}, 2, 2},
	{"ScrapSpear", {Scrap}, 1, 2},
	{"ScrapAxe", {Scrap, Heavy}, 1, 3},
};
assert_enum(itemi, ScrapAxe)