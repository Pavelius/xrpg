#include "bsreq.h"
#include "main.h"
#include "tablecsv.h"
#include "variant.h"

#define XKIND(T, V) template<> struct bsmeta<T> : bsmeta<V> {};\
template<> struct bsdata<T> : bsdata<V> {};

NOBSDATA(abilityi)
NOBSDATA(flagable<1>)
NOBSDATA(flagable<2>)
NOBSDATA(flagable<4>)
NOBSDATA(scenariotiles)
NOBSDATA(variants)
XKIND(gender_s, genderi)

BSMETA(variant) = {{}};
BSDATAD(variant)

BSMETA(tablecsvi) = {{}};
BSMETA(variants) = {{}};

static bool find_by_index(variant& result, const varianti& context, const char* id) {
	auto n = zlen(context.id);
	if(memcmp(id, context.id, n) == 0 && isnum(id[n])) {
		auto index = 0; stringbuilder::read(id + n, index);
		result.type = (variant_s)(&context - bsdata<varianti>::elements);
		result.value = index;
		return true;
	}
	return false;
}

BSMETA(point) = {
	BSREQ(x), BSREQ(y),
	{}};
BSDATAD(point)
BSMETA(menui) = {
	BSREQ(id),
	BSREQ(parent), BSREQ(resid), BSREQ(type),
	{}};
BSDATAD(menui)
BSMETA(actionbonusi) = {
	BSREQ(id),
	BSREQ(action), BSREQ(bonus),
	{}};
BSMETA(cardi) = {
	BSREQ(id),
	BSREQ(level), BSREQ(owner),
	BSREQ(initiative),
	BSREQ(upper), BSREQ(lower),
	{}};
BSDATAD(cardi)
BSMETA(monstercardi) = {
	BSREQ(id),
	BSREQ(initiative), BSREQ(abilities),
	{}};
BSDATAD(monstercardi)
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
BSMETA(feati) = {
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
BSMETA(monsteri) = {
	BSREQ(id),
	BSREQ(abilities), BSREQ(normal), BSREQ(elite),
	{}};
BSDATAD(monsteri)
BSMETA(scenariotiles) = {
	BSREQ(tile), BSREQ(position), BSREQ(inverse),
	{}};
BSMETA(scenarioi) = {
	BSREQ(id),
	BSREQ(starts), BSREQ(tiles),
	{}};
BSDATAD(scenarioi)
BSDATAD(activity)
BSMETA(actioni) = {BSREQ(id), {}};
BSMETA(areai) = {BSREQ(id), {}};
BSMETA(elementi) = {BSREQ(id), {}};
BSMETA(gamepropertyi) = {BSREQ(id), {}};
BSMETA(genderi) = {BSREQ(id), {}};
BSMETA(statei) = {BSREQ(id), {}};
BSMETA(tilei) = {BSREQ(id), {}};
BSMETA(varianti) = {BSREQ(id), {}};
#define VRSTD(T) bsmeta<T>::meta, bsdata<T>::source_ptr, 1
#define VRNOM(T) 0, bsdata<T>::source_ptr
BSDATA(varianti) = {
	{"NoVariant"},
	{"Action", VRSTD(actioni)},
	{"ActionBonus", VRSTD(actionbonusi)},
	{"Area", VRSTD(areai)},
	{"Card", VRSTD(cardi)},
	{"CardType", VRSTD(cardtypei)},
	{"CombatCard", VRSTD(combatcardi)},
	{"City", 0, &city_events, 1, FG(varianti::NotFoundByName)},
	{"Duration", VRSTD(durationi)},
	{"Element", VRSTD(elementi)},
	{"Feat", VRSTD(feati)},
	{"GameProperty", VRSTD(gamepropertyi)},
	{"Menu", VRSTD(menui)},
	{"Monster", VRSTD(monsteri)},
	{"MonsterCard", VRSTD(monstercardi)},
	{"Object", VRNOM(object)},
	{"Player", VRSTD(playeri)},
	{"Scenario", VRSTD(scenarioi)},
	{"State", VRSTD(statei)},
	{"SummonedCreature", VRSTD(summoni)},
	{"Tile", VRSTD(tilei)},
	{"Trap", VRSTD(trapi)},
	{"Type", VRSTD(varianti)},
};
assert_enum(varianti, Type)