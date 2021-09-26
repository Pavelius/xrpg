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

BSMETA(variant) = {{}};
BSDATAD(variant)

BSMETA(tablecsvi) = {{}};
BSMETA(variants) = {{}};

BSMETA(menui) = {
	BSREQ(id),
	BSREQ(parent), BSREQ(resid), BSREQ(type),
	{}};
BSDATAD(menui)
BSMETA(actioni) = {
	BSREQ(id),
	{}};
BSMETA(actionbonusi) = {
	BSREQ(id),
	BSREQ(action), BSREQ(bonus),
	{}};
BSDATAD(actionbonusi)
BSMETA(areai) = {
	BSREQ(id),
	{}};
BSMETA(cardi) = {
	BSREQ(id),
	BSREQ(level), BSREQ(owner),
	BSREQ(initiative),
	BSREQ(upper), BSREQ(lower),
	{}};
BSDATAD(cardi)
BSMETA(cardtypei) = {
	BSREQ(id),
	{}};
BSMETA(durationi) = {
	BSREQ(id),
	{}};
BSMETA(elementi) = {
	BSREQ(id),
	{}};
BSMETA(feati) = {
	BSREQ(id),
	{}};
BSMETA(genderi) = {
	BSREQ(id),
	{}};
BSMETA(playeri) = {
	BSREQ(id),
	BSREQ(health),
	{}};
BSDATAD(playeri)
BSMETA(statei) = {
	BSREQ(id),
	{}};
BSMETA(varianti) = {
	BSREQ(id),
	{}};
#define VRSTD(T) bsmeta<T>::meta, bsdata<T>::source_ptr
BSDATA(varianti) = {
	{"NoVariant"},
	{"Action", VRSTD(actioni)},
	{"ActionBonus", VRSTD(actionbonusi)},
	{"Area", VRSTD(areai)},
	{"Card", VRSTD(cardi)},
	{"CardType", VRSTD(cardtypei)},
	{"Duration", VRSTD(durationi)},
	{"Element", VRSTD(elementi)},
	{"Feat", VRSTD(feati)},
	{"Menu", VRSTD(menui)},
	{"Player", VRSTD(playeri)},
	{"State", VRSTD(statei)},
	{"Type", VRSTD(varianti)},
};
assert_enum(varianti, Type)