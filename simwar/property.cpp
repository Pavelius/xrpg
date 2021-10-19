#include "bsreq.h"
#include "main.h"

#define LNK(V, T) template<> struct bsmeta<V> : bsmeta<T> {};

LNK(stat_s, stati)
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
BSDATAD(resourcei)
BSMETA(landscapei) = {
    BSREQ(id),
    BSARR(income, KindDSet, char, bsdata<costi>::source_ptr),
    {}};
BSDATAD(landscapei)
BSMETA(uniti) = {
    BSREQ(id),
    BSARR(stats, KindDSet, char, bsdata<stati>::source_ptr),
    BSARR(cost, KindDSet, char, bsdata<costi>::source_ptr),
    BSARR(upkeep, KindDSet, char, bsdata<costi>::source_ptr),
{}};
BSDATAD(uniti)
BSMETA(bonusi) = {
    BSREQ(id),
    BSREQ(type), BSREQ(bonus),
    {}};
BSMETA(troop) = {
    BSREQ(type), BSREQ(owner),
    {}};
BSDATAD(troop)
BSMETA(playeri) = {
    BSREQ(id),
    {}};
BSMETA(prefixi) = {
    BSREQ(id),
    {}};
BSMETA(provincei) = {
    BSREQ(id),
    {}};
BSDATAD(provincei)
BSMETA(eventi) = {
    BSREQ(id),
    {}};

#define VRSTD(T) bsmeta<T>::meta, bsdata<T>::source_ptr, 0
#define VRNOM(T) 0, bsdata<T>::source_ptr
BSDATA(varianti) = {
	{"NoVariant"},
    {"Bonus", VRSTD(bonusi), 1},
    {"Cost", VRSTD(costi), 1},
    {"Event", VRSTD(eventi), 1},
    {"Landscape", VRSTD(landscapei), 1},
    {"Player", VRSTD(playeri), 1},
    {"Prefix", VRSTD(prefixi), 1},
    {"Resource", VRSTD(resourcei), 1},
    {"Stat", VRSTD(stati), 1},
    {"Troop", VRSTD(troop), 2},
    {"Unit", VRSTD(uniti), 1},
};
assert_enum(varianti, Unit)