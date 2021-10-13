#include "bsreq.h"
#include "main.h"

NOBSDATA(statable)

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
BSMETA(uniti) = {
    BSREQ(id),
    BSARR(stats, KindDSet, char, bsdata<stati>::source_ptr),
    BSARR(cost, KindDSet, char, bsdata<costi>::source_ptr),
    BSARR(upkeep, KindDSet, char, bsdata<costi>::source_ptr),
{}};
BSDATAD(uniti)

#define VRSTD(T) bsmeta<T>::meta, bsdata<T>::source_ptr
#define VRNOM(T) 0, bsdata<T>::source_ptr
BSDATA(varianti) = {
	{"NoVariant"},
	{"Resources", VRSTD(resourcei)},
	{"Units", VRSTD(uniti)},
};
assert_enum(varianti, Unit)