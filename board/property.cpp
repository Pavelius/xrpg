#include "bsreq.h"
#include "main.h"
#include "variant.h"

NOBSDATA(variants)

BSMETA(abilityi) = {
	BSREQ(id),
	BSREQ(priority),
	{}};
BSDATAC(abilityi, 128);
BSMETA(creaturei) = {
	BSREQ(id),
	{}};
BSDATAC(creaturei, 64);
BSMETA(racei) = {
	BSREQ(id),
	BSREQ(base), BSREQ(feats),
	{}};
BSDATAC(racei, 32);
BSMETA(feati) = {
	BSREQ(id),
	{}};
BSDATAC(feati, 128);
BSMETA(skilli) = {
	BSREQ(id),
	{}};
BSDATAC(skilli, 32);
BSMETA(modifieri) = {
	BSREQ(id),
	{}};
BSMETA(variant) = {
	{}};
BSDATAD(variant)

BSMETA(variants) = {{}};

#define VRSTD(T)  bsmeta<T>::meta, bsdata<T>::source_ptr
BSDATA(varianti) = {
	{"NoVariant"},
	{"Abilities", VRSTD(abilityi)},
	{"Creatures", VRSTD(creaturei)},
	{"Feats", VRSTD(feati)},
	{"Modifiers", VRSTD(modifieri)},
	{"Races", VRSTD(racei)},
	{"Skills", VRSTD(skilli)},
};
assert_enum(varianti, Skills)