#include "bsreq.h"
#include "main.h"
#include "tablecsv.h"
#include "variant.h"

#define XKIND(T, V) template<> struct bsmeta<T> : bsmeta<V> {};\
template<> struct bsdata<T> : bsdata<V> {};

NOBSDATA(abilityi)
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
BSMETA(combatcardi) = {
	BSREQ(id),
	BSREQ(bonus), BSREQ(count), BSREQ(owner), BSREQ(feats),
	{}};
BSDATAD(combatcardi)
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
	BSREQ(gender),
	BSREQ(health),
	{}};
BSDATAD(playeri)
BSMETA(summoni) = {
	BSREQ(id),
	BSREQ(hits), BSREQ(move), BSREQ(attack), BSREQ(range),
	BSREQ(feats),
	{}};
BSDATAD(summoni)
BSMETA(trapi) = {
	BSREQ(id),
	BSREQ(damage),
	BSREQ(feats),
	{}};
BSDATAD(trapi)
BSMETA(abilityi) = {
	BSREQ(hits), BSREQ(move), BSREQ(attack), BSREQ(range),
	BSREQ(feats),
	{}};
BSMETA(fractioni) = {
	BSREQ(id),
	{}};
BSMETA(gamepropertyi) = {
	BSREQ(id),
	{}};
BSMETA(monsteri) = {
	BSREQ(id),
	BSREQ(normal), BSREQ(elite),
	{}};
BSDATAD(monsteri)
BSMETA(statei) = {
	BSREQ(id),
	{}};
BSMETA(varianti) = {
	BSREQ(id),
	{}};
#define VRSTD(T) bsmeta<T>::meta, bsdata<T>::source_ptr
#define VRNOM(T) 0, bsdata<T>::source_ptr
BSDATA(varianti) = {
	{"NoVariant"},
	{"Action", VRSTD(actioni)},
	{"ActionBonus", VRSTD(actionbonusi)},
	{"Area", VRSTD(areai)},
	{"Card", VRSTD(cardi)},
	{"CardType", VRSTD(cardtypei)},
	{"CombatCard", VRSTD(combatcardi)},
	{"Duration", VRSTD(durationi)},
	{"Element", VRSTD(elementi)},
	{"Feat", VRSTD(feati)},
	{"Fraction", VRSTD(fractioni)},
	{"GameProperty", VRSTD(gamepropertyi)},
	{"Menu", VRSTD(menui)},
	{"Monster", VRSTD(monsteri)},
    {"Object", VRNOM(object)},
	{"Player", VRSTD(playeri)},
	{"State", VRSTD(statei)},
	{"SummonedCreature", VRSTD(summoni)},
	{"Trap", VRSTD(trapi)},
	{"Type", VRSTD(varianti)},
};
assert_enum(varianti, Type)