#include "answers.h"
#include "crt.h"
#include "color.h"
#include "dice.h"
#include "flagable.h"
#include "gender.h"
#include "posable.h"
#include "stringbuilder.h"
#include "variantlist.h"

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
enum component_s : unsigned char {
	Verbal, Somatic, Manual,
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
	Prone,
	Hostile, Locale, Summoned,
};
enum state_s : unsigned char {
	Alive, NotWounded, LightWounded, HeavyWounded, Defeated,
	Poisoned, Restrained,
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
	Ability, Advance, Alignment, Class, Damage, Item, ItemTag,
	Modifier, Pack, Race, Special, Skill,
};
enum action_s : unsigned char {
	StandartAction, MoveAction, BonusAction,
};
class creature;
typedef flagable<1 + Poison / 8> damagea;
typedef flagable<1 + Persuasion / 8> skilla;
typedef flagable<1 + Lucky / 8> speciala;
typedef flagable<16> itemf;
typedef flagable<32> spellf;
typedef flagable<4> itemtgf;
typedef char abilitya[DamageRanged + 1];
struct nameablei {
	const char*			id;
	const char*			getid() const { return id; }
	const char*			getname() const { return getnm(id); }
};
struct abilityi {
	const char*			id;
	ability_s			parent;
};
struct skilli {
	const char*			id;
};
struct alignmenti : nameablei {
};
struct classi : nameablei {
	ability_s			best;
	char				hd;
};
struct modifieri {
	const char*			id;
};
struct racei : nameablei {
	variant				parent;
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
struct speciali {
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
	const itemi&		geti() const { return bsdata<itemi>::elements[type]; }
	creature*			getowner() const;
	void				use();
};
struct statable {
	abilitya			abilities;
	damagea				resistance, immunity, vulnerability;
	speciala			special;
	spellf				known_spells;
	int					hp, hp_maximum;
	static void			copy(statable& dest, const statable& source);
	void				add(ability_s i, int v) { abilities[i] += v; }
	void				random_ability(classi& kind);
	static int			roll(int advantage, bool lucky);
	static int			roll(dice_s v);
};
struct actable : nameablei {
	gender_s			gender;
	void				actv(stringbuilder& sb, const char* format, const char* format_param);
};
class creature : public actable, public statable, public posable {
	unsigned char		race, kind, level, alignment;
	statable			basic;
	item				wears[LastBackpack + 1];
	char				avatar[12];
	void				create_finish();
	void				update_finish();
public:
	explicit operator bool() const { return avatar[0] != 0; }
	bool				attack(creature& enemy, int advantages, int bonus);
	bool				attack(ability_s attack_type, creature& enemy, item& weapon, int advantages, int bonus);
	void				clear();
	void				create(racei& race, classi& kind, gender_s gender);
	void				damage(damage_s type, int value);
	void				fight();
	void				fixattack(point goal, ability_s type);
	void				fixdamage(int value) const;
	void				fixmiss();
	int					get(ability_s i) const { return abilities[i]; }
	const item&			get(wear_s i) const { return wears[i]; }
	item&				get(wear_s i) { return wears[i]; }
	int					getbonus(ability_s i) const { return get(i) / 2 - 5; }
	const classi&		getclass() const { return bsdata<classi>::elements[kind]; }
	int					gethd() const;
	bool				is(state_s v) const;
	constexpr bool		is(special_s v) const { return special.is(v); }
	constexpr bool		ismonster() const { return kind==0; }
	void				levelup();
	void				lookmove();
	void				paint() const;
	void				setavatar(const char* v);
	void				update();
};
struct creaturea : public adat<creature*, 64> {
	void				select();
};
class gamei {
};
namespace draw {
point					m2s(int x, int y);
point					s2m(point p);
void					startmenu();
void					waitanimation();
}