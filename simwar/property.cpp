#include "bsreq.h"
#include "main.h"

#define LNK(V, T) template<> struct bsmeta<V> : bsmeta<T> {};\
template<> struct bsdata<V> : bsdata<T> {};

LNK(stat_s, stati)
NOBSDATA(point)
NOBSDATA(crange)

BSMETA(variant) = {{}};
BSMETA(varianti) = {BSREQ(id), {}};
BSDATAD(variant)

BSMETA(stati) = {
	BSREQ(id),
	{}};
BSMETA(costi) = {
	BSREQ(id),
	{}};
BSMETA(resourcei) = {
	BSREQ(id),
	BSDST(cost, costi),
	{}};
BSDATAC(resourcei, 16)
BSMETA(populationi) = {
	BSREQ(id),
	BSREQ(level),
	{}};
BSDATAC(populationi, 16)
BSMETA(landscapei) = {
	BSREQ(id),
	BSREQ(explored),
	BSDST(income, costi),
	{}};
BSMETA(uniti) = {
	BSREQ(id),
	BSREQ(nation),
	BSDST(stats, stati),
	BSDST(cost, costi),
	BSDST(upkeep, costi),
	BSFLG(tags, tagi),
	BSADT(need, resourcei),
	{}};
BSDATAC(uniti, 128)
BSMETA(bonusi) = {
	BSREQ(id),
	BSREQ(type), BSREQ(bonus),
	{}};
BSMETA(playeri) = {
	BSREQ(id),
	BSREQ(avatar),
	{}};
BSMETA(prefixi) = {
	BSREQ(id),
	{}};
BSMETA(provincei) = {
	BSREQ(id),
	BSREQ(position),
	BSREQ(player),
	BSDST(income, costi),
	BSDST(stats, stati),
	BSREQ(landscape),
	BSREQ(neightboards),
	BSFLG(buildings, buildingi),
	{}};
BSDATAC(provincei, 256)
BSMETA(eventi) = {
	BSREQ(id),
	{}};
BSMETA(buildingi) = {
	BSREQ(id),
	BSREQ(base), BSREQ(effect), BSREQ(condition),
	BSDST(cost, costi),
	{}};
BSDATAC(buildingi, 64)
BSMETA(point) = {
	BSREQ(x), BSREQ(y),
	{}};
BSMETA(crange) = {
	BSREQ(min), BSREQ(max),
	{}};
BSMETA(nationi) = {
	BSREQ(id),
	BSREQ(avatar), BSREQ(alignment), BSDST(income, costi),
	{}};
BSDATAC(nationi, 32)
BSMETA(tagi) = {
	BSREQ(id),
	{}};
BSMETA(tactici) = {
	BSREQ(id),
	BSREQ(bonus),
	BSFLG(disable, tactici),
	{}};
BSDATAC(tactici, 32)
BSMETA(heroi) = {
	BSREQ(id),
	BSREQ(avatar),
	BSREQ(nation),
	BSDST(stats, stati),
	BSDST(cost, costi),
	BSDST(upkeep, costi),
	BSREQ(province),
	{}};
BSDATAC(heroi, 128)

#define VRSTD(T) bsmeta<T>::meta, bsdata<T>::source_ptr, 1
#define VRNOM(T) 0, bsdata<T>::source_ptr
BSDATA(varianti) = {
	{"NoVariant"},
	{"Bonus", VRSTD(bonusi)},
	{"Building", VRSTD(buildingi)},
	{"Cost", VRSTD(costi)},
	{"Event", VRSTD(eventi)},
	{"Hero", VRSTD(heroi)},
	{"Landscape", VRSTD(landscapei)},
	{"Nation", VRSTD(nationi)},
	{"Player", VRSTD(playeri)},
	{"Prefix", VRSTD(prefixi)},
	{"Province", VRSTD(provincei)},
	{"Resource", VRSTD(resourcei)},
	{"Stat", VRSTD(stati)},
	{"Tactic", VRSTD(tactici)},
	{"Tag", VRSTD(tactici)},
	{"Unit", VRSTD(uniti)},
};
assert_enum(varianti, Unit)