#include "bsreq.h"
#include "main.h"
#include "tablecsv.h"
#include "variant.h"

NOBSDATA(variants)
NOBSDATA(flagable<1>)
NOBSDATA(flagable<2>)
NOBSDATA(flagable<4>)

BSMETA(abilityi) = {
	BSREQ(id),
	{}};
BSDATAC(abilityi, 128);
BSMETA(parameteri) = {
	BSREQ(id),
	BSREQ(base),
	BSREQ(content),
	{}};
BSDATAC(parameteri, 128);
BSMETA(packagei) = {
	BSREQ(id),
	BSREQ(content),
	{}};
BSDATAC(packagei, 64);
BSMETA(classi) = {
	BSREQ(id),
	BSREQ(spellprogress),
	BSREQ(feats),
	BSREQ(skills),
	BSREQ(hitdice),
	{}};
BSDATAC(classi, 32);
BSMETA(creaturei) = {
	BSREQ(id),
	{}};
BSDATAC(creaturei, 64);
BSMETA(racei) = {
	BSREQ(id),
	BSREQ(base),
	BSREQ(feats),
	{}};
BSDATAC(racei, 32);
BSMETA(feati) = {
	BSREQ(id),
	BSREQ(effect),
	{}};
BSDATAC(feati, 128);
BSMETA(skilli) = {
	BSREQ(id),
	{}};
BSDATAC(skilli, 32);
BSMETA(modifieri) = {
	BSREQ(id),
	{}};
BSMETA(variant) = {{}};
BSDATAD(variant)

BSMETA(tablecsvi) = {{}};
BSMETA(variants) = {{}};

#define VRSTD(T) bsmeta<T>::meta, bsdata<T>::source_ptr
BSDATA(varianti) = {
	{"NoVariant"},
	{"Abilities", VRSTD(abilityi)},
	{"Classes", VRSTD(classi)},
	{"Creatures", VRSTD(creaturei)},
	{"Feats", VRSTD(feati)},
	{"Modifiers", VRSTD(modifieri)},
	{"Packages", VRSTD(packagei)},
	{"Parameters", VRSTD(parameteri)},
	{"Progress"},
	{"Races", VRSTD(racei)},
	{"Skills", VRSTD(skilli)},
	{"Tables", VRSTD(tablecsvi)},
};
assert_enum(varianti, Tables)