#include "crt.h"
#include "flagable.h"
#include "variant.h"

#pragma once

enum ability_s : unsigned char {
	HP, Damage, Armor, Pierce,
	Strenght, Dexterity, Constitution, Intellegence, Wisdow, Charisma,
};
enum tag_s : unsigned char {
	Arrows, CharismaMinus, Clumsy, Dangerous, Forceful, IgnoreArmor, Messy, NeedArrows, Precise, Ration, Reload, Slow, Stun, Thrown, TwoHanded,
	CanHealDebility, CanHealPoison,
	Hand, Close, Reach, Near, Far,
};
enum itemuf_s : unsigned char {
	HooksAndSpikes, Sharp, PerfectlyWeigthed, SerratedEdges, Glows, Huge, Versatile, WellCrafted,
};
enum wear_s : unsigned char {
	Head, Body, LeftHand, RightHand, LeftRing, RightRing, Legs,
	Backpack, LastBackpack = Backpack + 16,
};
enum variant_s : unsigned char {
	NoVariant, Item, Tag,
};

typedef flagable<1> itemufa;
typedef flagable<1 + Far / 8> taga;

struct tagi {
	const char* id;
};
struct itemi {
	const char* id;
	wear_s slot;
	taga tags;
	char coins;
	char weight, damage, pierce, armor, uses, heal;
};
union item {
	unsigned u;
	short unsigned w[2];
	struct {
		unsigned char type;
		unsigned char signature : 1;
		unsigned char used : 3;
		itemufa feats;
	};
	constexpr item() : u(0) {}
	constexpr item(unsigned char type) : type(0), signature(0), used(0), feats() {}
	constexpr const itemi& geti() const { return bsdata<itemi>::elements[type]; }
	constexpr bool is(itemuf_s v) const { return feats.is(v); }
	constexpr bool is(tag_s v) const { return geti().tags.is(v); }
};
typedef item weara[LastBackpack + 1];
typedef char abilitya[HP + 1];
struct statable {
	abilitya	abilities;
};
class creature : public statable {
	statable	basic;
public:
	int			get(ability_s v) const { return abilities[v]; }
	int			getbonus(ability_s v) const { return abilities[v] / 2 - 5; }
};