#include "bsreq.h"
#include "main.h"

#define LNK(V, T) template<> struct bsmeta<V> : bsmeta<T> {};\
template<> struct bsdata<V> : bsdata<T> {};

BSMETA(variant) = {{}};
BSMETA(varianti) = {BSREQ(id), {}};
BSDATAD(variant)

BSMETA(actioni) = {
	BSREQ(id),
	{}};
BSMETA(tagi) = {
	BSREQ(id),
	{}};
BSMETA(movei) = {
	BSREQ(id),
	{}};
BSMETA(itemi) = {
	BSREQ(id), BSREQ(weight), BSREQ(coins), BSREQ(damage), BSREQ(armor), BSREQ(uses), BSREQ(pierce), BSREQ(heal), BSREQ(forward),
	BSFLG(tags, tagi),
	BSFLG(moves, movei),
	{}};
BSDATAC(itemi, 256)

#define VRSTD(T) bsmeta<T>::meta, bsdata<T>::source_ptr, 1
BSDATA(varianti) = {
	{"NoVariant"},
	{"Action", VRSTD(actioni)},
	{"Item", VRSTD(itemi)},
	{"Move", VRSTD(movei)},
	{"Tag", VRSTD(tagi)},
};
assert_enum(varianti, Tag)