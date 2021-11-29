#include "bsreq.h"
#include "main.h"

#define XKIND(T, V) template<> struct bsmeta<T> : bsmeta<V> {};\
template<> struct bsdata<T> : bsdata<V> {};

NOBSDATA(attacki)
NOBSDATA(variants)

XKIND(ability_s, abilityi)
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
	BSREQ(id), BSREQ(best),
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
	BSREQ(id), BSREQ(parent),
	{}};
BSDATAC(racei, 32)
BSMETA(skilli) = {
	BSREQ(id),
	{}};
BSMETA(speciali) = {
	BSREQ(id),
	{}};
BSMETA(creature) = {
	{}};
BSDATAC(creature, 1024)

#define VRSTD(T, KN) bsmeta<T>::meta, bsdata<T>::source_ptr, KN
BSDATA(varianti) = {
	{"NoVariant"},
	{"Ability", VRSTD(abilityi, 1)},
	{"Advance", VRSTD(advancei, 3)},
	{"Alignment", VRSTD(abilityi, 1)},
	{"Class", VRSTD(classi, 1)},
	{"Damage", VRSTD(damagei, 1)},
	{"Item", VRSTD(itemi, 1)},
	{"ItemTag", VRSTD(itemtgi, 1)},
	{"Modifier", VRSTD(modifieri, 1)},
	{"Pack", VRSTD(abilityi, 1)},
	{"Race", VRSTD(racei, 1)},
	{"Special", VRSTD(speciali, 1)},
	{"Skill", VRSTD(abilityi, 1)},
};
assert_enum(varianti, Skill)