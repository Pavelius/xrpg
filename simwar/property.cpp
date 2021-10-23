#include "bsreq.h"
#include "main.h"

#define LNK(V, T) template<> struct bsmeta<V> : bsmeta<T> {};\
template<> struct bsdata<V> : bsdata<T> {};

LNK(stat_s, stati)
NOBSDATA(point)
NOBSDATA(range)

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
    BSREQ(avatar),
    BSREQ(nation),
    BSDST(stats, stati),
    BSDST(cost, costi),
    BSDST(upkeep, costi),
    BSFLG(encounter, landscapei), BSREQ(encounter_tought), BSREQ(encounter_monster),
    BSADT(need, resourcei),
    {}};
BSDATAC(uniti, 128)
BSMETA(bonusi) = {
    BSREQ(id),
    BSREQ(type), BSREQ(bonus),
    {}};
BSMETA(troop) = {
    BSREQ(uid),
    BSREQ(type), BSREQ(province),
    {}};
BSDATAC(troop, 2048)
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
    BSREQ(owner),
    BSDST(income, costi),
    BSDST(stats, stati),
    BSREQ(landscape),
    BSREQ(neightboards),
    {}};
BSDATAC(provincei, 256)
BSMETA(eventi) = {
    BSREQ(id),
    {}};
BSMETA(point) = {
    BSREQ(x), BSREQ(y),
    {}};
BSMETA(actioni) = {
    BSREQ(id),
    {}};
BSMETA(range) = {
    BSREQ(min), BSREQ(max),
    {}};
BSMETA(nationi) = {
    BSREQ(id),
    {}};
BSDATAC(nationi, 32)
BSMETA(hero) = {
    BSREQ(id),
    BSREQ(avatar),
    BSREQ(nation),
    BSDST(stats, stati),
    BSDST(cost, costi),
    BSDST(upkeep, costi),
    BSFLG(encounter, landscapei), BSREQ(encounter_tought), BSREQ(encounter_monster),
    BSREQ(province),
    {}};
BSDATAC(hero, 128)

#define VRSTD(T) bsmeta<T>::meta, bsdata<T>::source_ptr, 0, 1
#define VRNOM(T) 0, bsdata<T>::source_ptr
BSDATA(varianti) = {
	{"NoVariant"},
    {"Action", VRSTD(actioni)},
    {"Bonus", VRSTD(bonusi)},
    {"Cost", VRSTD(costi)},
    {"Event", VRSTD(eventi)},
    {"Hero", VRSTD(hero)},
    {"Landscape", VRSTD(landscapei)},
    {"Nation", VRSTD(nationi)},
    {"Player", VRSTD(playeri)},
    {"Prefix", VRSTD(prefixi)},
    {"Province", VRSTD(provincei)},
    {"Resource", VRSTD(resourcei)},
    {"Stat", VRSTD(stati)},
    {"Troop", VRSTD(troop)},
    {"Unit", VRSTD(uniti)},
};
assert_enum(varianti, Unit)