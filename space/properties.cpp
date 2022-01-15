#include "bsreq.h"
#include "main.h"

#define XKIND(T, V) template<> struct bsmeta<T> : bsmeta<V> {};\
template<> struct bsdata<T> : bsdata<V> {};

NOBSDATA(variants)

BSMETA(variant) = {{}};
BSMETA(varianti) = {BSREQ(id), {}};
BSDATAD(variant)

BSMETA(damagei) = {
	BSREQ(id),
	{}};
BSMETA(squadi) = {
	BSREQ(id),
	BSREQ(combat), BSREQ(infiltrate), BSREQ(research), BSREQ(heal), BSREQ(count),
	BSFLG(inflict, damagei),
	{}};
BSDATAC(squadi, 128)

#define VRSTD(T, KN) bsmeta<T>::meta, bsdata<T>::source_ptr, KN
BSDATA(varianti) = {
	{"NoVariant"},
	{"Damage", VRSTD(damagei, 1)},
	{"Squad", VRSTD(squadi, 1)},
};
assert_enum(varianti, Squad)