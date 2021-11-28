#include "bsreq.h"
#include "main.h"

#define XKIND(T, V) template<> struct bsmeta<T> : bsmeta<V> {};\
template<> struct bsdata<T> : bsdata<V> {};

NOBSDATA(attacki)
NOBSDATA(variants)

XKIND(gender_s, genderi)
XKIND(damage_s, damagei)
XKIND(dice_s, dicei)

BSMETA(variant) = {{}};
BSMETA(varianti) = {BSREQ(id), {}};
BSDATAD(variant)

BSMETA(abilityi) = {
	BSREQ(id),
	{}};
BSMETA(advancei) = {
	BSREQ(object), BSREQ(level), BSREQ(id),
	BSREQ(choose_count), BSREQ(traits),
	{}};
BSDATAC(advancei, 1024)
BSMETA(attacki) = {
	BSREQ(attack_bonus), BSREQ(damage_bonus),
	BSREQ(type), BSREQ(damage),
	{}};
BSMETA(alignmenti) = {
	BSREQ(id),
	{}};
BSDATAC(alignmenti, 16)
BSMETA(classi) = {
	BSREQ(id),
	{}};
BSDATAC(classi, 32)
BSMETA(damagei) = {
	BSREQ(id),
	{}};
BSMETA(dicei) = {
	BSREQ(id),
	{}};
BSMETA(itemi) = {
	BSREQ(id),
	BSREQ(cost), BSREQ(weight), BSREQ(ac),
	BSREQ(attack),
	BSFLG(tags, itemtgi),
	{}};
BSDATAC(itemi, 256)
BSMETA(itemtgi) = {
	BSREQ(id),
	{}};
BSMETA(modifieri) = {
	BSREQ(id),
	{}};
BSMETA(packi) = {
	BSREQ(id),
	BSREQ(content),
	{}};
BSMETA(racei) = {
	BSREQ(id),
	{}};
BSDATAC(racei, 32)
BSMETA(skilli) = {
	BSREQ(id),
	{}};
BSMETA(creature) = {
	{}};
BSDATAC(creature, 1024)

#define VRSTD(T) bsmeta<T>::meta, bsdata<T>::source_ptr, 1
#define VRSPC(T, N) bsmeta<T>::meta, bsdata<T>::source_ptr, N
BSDATA(varianti) = {
	{"NoVariant"},
	{"Ability", VRSTD(abilityi)},
	{"Advance", VRSPC(advancei, 3)},
	{"Alignment", VRSTD(abilityi)},
	{"Class", VRSTD(classi)},
	{"Damage", VRSTD(damagei)},
	{"Item", VRSTD(itemi)},
	{"ItemTag", VRSTD(itemtgi)},
	{"Modifier", VRSTD(modifieri)},
	{"Pack", VRSTD(abilityi)},
	{"Race", VRSTD(racei)},
	{"Special"},
	{"Skill", VRSTD(abilityi)},
};
assert_enum(varianti, Skill)