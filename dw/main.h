#include "crt.h"
#include "flagable.h"
#include "gender.h"
#include "variant.h"

#pragma once

enum ability_s : unsigned char {
	HP, Damage, Armor, Pierce,
	Strenght, Dexterity, Constitution, Intellegence, Wisdow, Charisma,
};
enum race_s : unsigned char {
	Human, Dwarf, Elf, Halfling,
};
enum tag_s : unsigned char {
	Arrows, CharismaMinus, Clumsy, Dangerous, Forceful, IgnoreArmor, Messy, NeedArrows, Precise, Ration, Reload, Slow, Stun, Thrown, TwoHanded,
	HealDebility, HealPoison,
	UsableGear,
	Hand, Close, Reach, Near, Far,
};
enum itemuf_s : unsigned char {
	HooksAndSpikes, Sharp, PerfectlyWeigthed, SerratedEdges, Glows, Huge, Versatile, WellCrafted,
};
enum wear_s : unsigned char {
	Head, Body, RightHand, LeftHand, RightRing, LeftRing, Legs,
	Backpack, LastBackpack = Backpack + 16,
};
enum move_s : unsigned char {
	HackAndSlash, Volley, DefyDanger, DefyPoison, Defend, SpoutLore, DiscernRealities, Parley,
	Aid, Interfere, LastBreath, Encumbrance,
	MakeCamp, TakeWatch, UndertakeAPerilousJourney, LevelUp, EndOfSession,
	Carouse, Supply, Recover, Recruit, OutstandingWarrants, Bolster,
};
enum action_s : unsigned char {
	SufferDamage, InflictDamage,
	UseAmmo, UseGear, UseRation, ForgetSpell,
	BreakItem, BreakObject,
};
enum variant_s : unsigned char {
	NoVariant,
	Action, Item, Move, Tag,
};
enum result_s : unsigned char {
	Fail, PartialSuccess, Success, CriticalSuccess,
};

typedef flagable<1> itemufa;
typedef flagable<1 + Far / 8> taga;
typedef flagable<1 + Carouse / 8> movea;
typedef flagable<1 + Halfling / 8> racea;
typedef char abilitya[Charisma + 1];

struct racei {
	const char* id;
};
struct tagi {
	const char* id;
};
struct actioni {
	const char* id;
};
struct resulti {
	const char* name;
	int options;
	variants effect;
};
struct optioni {
	const char* id;
	int choose_count;
	variants actions;
};
struct movei {
	const char* id;
	ability_s ability;
	resulti results[4];
};
struct moveable {
	movea moves;
	char forward;
	bool is(move_s v) const { return moves.is(v); }
};
struct itemi : moveable {
	const char* id;
	wear_s slot;
	taga tags;
	racea need;
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
struct statable : moveable {
	abilitya abilities;
	static result_s last_result;
	static int last_roll;
	static result_s rollv(int bonus);
};
struct nameable {
	char name[16];
	race_s race;
	gender_s gender;
public:
	void act(const char* format, ...) { actv(format, xva_start(format)); }
	void actv(const char* format, const char* format_param);
	gender_s getgender() const { return gender; }
	const char* getname() const { return name; }
	race_s getrace() const { return race; }
};
class creature : public nameable, public statable {
	statable basic;
public:
	int	get(ability_s v) const { return abilities[v]; }
	int getbonus(ability_s v) const { return abilities[v] / 2 - 5; }
};
class gamei {
	char bolster;
public:
	int	getbolster() const { return bolster; }
	void usebolster() { bolster--; }
};