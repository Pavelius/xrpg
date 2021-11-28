#include "answers.h"
#include "crt.h"
#include "dice.h"
#include "flagable.h"
#include "gender.h"
#include "stringbuilder.h"
#include "variant.h"
#include "varianta.h"

#pragma once

enum ability_s : unsigned char {
	Strenght, Dexterity, Constitution, Intellect, Wisdow, Charisma,
	AttackAll, AttackMelee, AttackRanged,
	DamageAll, DamageMelee, DamageRanged,
	AC,
	SaveAll,
	SaveVsCharm, SaveVsDisease, SaveVsIllusion, SaveVsPoison, SaveVsTrap,
	Hits,
};
enum skill_s : unsigned char {
	Athletics,
	Acrobatics, SleightOfHand, Stealth,
	Arcana, History, Investigation, Nature, Religion,
	AnimalHandling, Insight, Medicine, Perception, Survival,
	Deception, Intimidation, Performance, Persuasion
};
enum dice_s : unsigned char {
	NoDice,
	D1d2, D1d3, D1d4, D1d6, D1d8, D1d10, D1d12, D1d20,
};
enum special_s : unsigned char {
	Brave, Darkvision, HeavyArmorNotRestrictSpeed, Stonecunning, Lucky,
	Locale, Summoned,
};
enum modifier_s : unsigned char {
	Proficiency, DoubleProficiency,
	Resistance, Immunity, Vulnerability,
	Advantage, Disadvantage
};
enum wear_s : unsigned char {
	Head, Neck, Body, Torso, RightHand, LeftHand, RightRing, LeftRing, Legs,
	Backpack, LastBackpack = Backpack + 4
};
enum damage_s : unsigned char {
	Acid, Bludgeon, Cold, Fire, Force, Lighting, Necrotic,
	Piercing, Poison, Psychic, Radiant, Slashing, Thunder
};
enum itemtg_s : unsigned char {
	Finess, Light, Thrown, TwoHanded, Versatile,
	DexterityBonus, DexterityRestricted, StealthDisadvantage,
	NeedHightStrenght, NeedVeryHightStrenght
};
enum variant_s : unsigned char {
	NoVariant,
	Ability, Advance, Alignment, Class, Damage, Item, ItemTag, Modifier, Pack, Race, Special, Skill,
};
typedef flagable<1 + Poison / 8> damagea;
typedef flagable<1 + Persuasion / 8> skilla;
typedef flagable<1 + Lucky / 8> speciala;
typedef flagable<16> itemf;
typedef flagable<32> spellf;
typedef flagable<4> itemtgf;
typedef char abilitya[DamageRanged + 1];
class creature;
struct nameablei {
	const char*			id;
	const char*			getid() const { return id; }
	const char*			getname() const { return getnm(id); }
};
struct abilityi {
	const char*			id;
};
struct skilli {
	const char*			id;
};
struct alignmenti : nameablei {
};
struct classi : nameablei {
};
struct modifieri {
	const char*			id;
};
struct racei : nameablei {
};
struct weari {
	const char*			id;
};
struct advancei : nameablei {
	variant				object;
	char				level;
	char				choose_count;
	variants			traits;
};
struct packi : nameablei {
	variants			content;
};
struct damagei {
	const char*			id;
};
struct enchanti : nameablei {
	int					magic;
	variants			powers;
};
struct dicei {
	const char*			id;
	dice				result;
};
struct attacki {
	char				attack_bonus, damage_bonus;
	damage_s			type;
	dice_s				damage;
};
struct itemtgi {
	const char*			id;
};
struct itemi : nameablei {
	int					weight;
	int					cost;
	attacki				attack;
	itemtgf				tags;
	char				ac;
};
union item {
	unsigned			u;
	unsigned short		uw[2];
	struct {
		unsigned char	type;
		unsigned char	power;
		//
		unsigned char	known_magic : 1;
		unsigned char	known_power : 1;
		unsigned char	cursed : 1;
		unsigned char	charges : 5;
		//
		unsigned char	broken : 2;
	};
	creature*			getowner() const;
	void				use();
};
struct statable {
	abilitya			abilities;
	damagea				resistance, immunity, vulnerability;
	speciala			special;
	spellf				known_spells;
	static void			copy(statable& dest, const statable& source);
	static int			roll(int advantage, bool lucky);
};
class creature : nameablei, public statable {
	statable			basic;
public:
	bool				attack(creature& enemy, wear_s slot, int advantages = 0, int bonus = 0);
	int					get(ability_s i) const { return abilities[i]; }
	constexpr bool		is(special_s v) const { return special.is(v); }
};
class gamei {
};