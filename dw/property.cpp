#include "bsreq.h"
#include "main.h"

#define LNK(V, T) template<> struct bsmeta<V> : bsmeta<T> {};\
template<> struct bsdata<V> : bsdata<T> {};

BSMETA(variant) = {{}};
BSMETA(varianti) = {BSREQ(id), {}};
BSDATAD(variant)

BSMETA(tagi) = {
	BSREQ(id),
	{}};
BSMETA(itemi) = {
	BSREQ(id), BSREQ(weight), BSREQ(coins), BSREQ(damage), BSREQ(armor), BSREQ(uses), BSREQ(pierce),
	BSFLG(tags, tagi),
	{}};
BSDATAC(itemi, 256)

#define VRSTD(T) bsmeta<T>::meta, bsdata<T>::source_ptr, 1
BSDATA(varianti) = {
	{"NoVariant"},
	{"Item", VRSTD(itemi)},
	{"Tag", VRSTD(tagi)},
};
assert_enum(varianti, Tag)