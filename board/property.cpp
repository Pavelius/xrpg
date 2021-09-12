#include "bsreq.h"
#include "main.h"
#include "tablecsv.h"
#include "variant.h"

#define XKIND(T, V) template<> struct bsmeta<T> : bsmeta<V> {};\
template<> struct bsdata<T> : bsdata<V> {};

NOBSDATA(variants)
NOBSDATA(flagable<1>)
NOBSDATA(flagable<2>)
NOBSDATA(flagable<4>)
XKIND(gender_s, genderi)

BSMETA(abilityi) = {
	BSREQ(id),
	{}};
BSDATAC(abilityi, 128);
BSMETA(parameteri) = {
	BSREQ(id),
	BSREQ(base),
	BSREQ(content),
	BSREQ(flags),
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
	BSREQ(name),
	BSREQ(race),
	BSREQ(gender),
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

BSMETA(menui) = {
	BSREQ(id),
	BSREQ(parent), BSREQ(resid), BSREQ(menuback),
	{}};
BSDATAD(menui)

BSMETA(recordi) = {
	BSREQ(id),
	BSREQ(parent), BSREQ(param),
	{}};
BSDATAD(recordi)

BSMETA(dicei) = {
	BSREQ(id),
	BSREQ(min), BSREQ(max),
	{}};
BSDATAC(dicei, 64);

BSMETA(inputi) = {
	BSREQ(id),
	BSREQ(requisit),
	BSREQ(resid),
	BSREQ(source),
	{}};
BSDATAC(inputi, 64);

BSMETA(conditioni) = {
	BSREQ(id),
	{}};
BSMETA(genderi) = {
	BSREQ(id),
	{}};

BSMETA(varianti) = {
	BSREQ(id),
	{}};
#define VRSTD(T) bsmeta<T>::meta, bsdata<T>::source_ptr
BSDATA(varianti) = {
	{"NoVariant"},
	{"Abilities", VRSTD(abilityi)},
	{"Classes", VRSTD(classi)},
	{"Conditions", VRSTD(conditioni)},
	{"Creatures", VRSTD(creaturei)},
	{"Dices", VRSTD(dicei)},
	{"Feats", VRSTD(feati)},
	{"Genders", VRSTD(genderi)},
	{"Input", VRSTD(inputi)},
	{"Menu", VRSTD(menui)},
	{"Modifiers", VRSTD(modifieri)},
	{"Packages", VRSTD(packagei)},
	{"Parameters", VRSTD(parameteri)},
	{"Progress"},
	{"Races", VRSTD(racei)},
	{"Records", VRSTD(recordi)},
	{"Skills", VRSTD(skilli)},
	{"Tables", VRSTD(tablecsvi)},
	{"Types", VRSTD(varianti)},
};
assert_enum(varianti, Types)