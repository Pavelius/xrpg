#include "answers.h"
#include "command.h"
#include "datamap.h"
#include "dataset.h"
#include "command.h"
#include "gender.h"
#include "menu.h"
#include "moveable.h"
#include "point.h"
#include "view.h"
#include "varianta.h"

#pragma once

enum variant_s : unsigned char {
	None, Attribute, Item, Gender, Location, Role, Skill,
};
enum attribute_s : unsigned char {
	Strenght, Agility, Wits, Empathy
};
enum skill_s : unsigned char {
	Endure, Force, Fight, Sneak, Move, Shoot, Scout, Comprehend, KnowTheZone, SenceEmotion, Manipulate, Heal, Intimidate,
};
enum item_s : unsigned char {
	NoItem,
	BluntInstrument, BicycleChain, BrassKnuckles, ScrapKnife, BaseballBat, BatWithSpikes, Machete, ScrapSpear, ScrapAxe,
};
enum role_s : unsigned char {
	Enforcer, Gearhead, Stalker, Fixer, DogHandler, Chronicler, Boss, Slave,
	Commoner,
};
enum talent_s : unsigned char {
	BargeThrought, MeanStreak, SuckerPunch,
	Inventor, Motorhead, Tinkerer,
	MonsterHunter, RotFinder, Scavenger,
	JuicyInfo, ViciousCreep, WheelerDealer,
	BloodHound, FightDog, MutantsBestFriend,
	Agitator, BoneSaw, Perfomer,
	Commander, Gunslingers, Racketeer,
	Cynic, Rebel, Resilent,
	Admirer, Archeologist, BadOmens, Bodyguard, Butcher,
	CombatVeteran, CoolHead, Counselor, Coward, FastDraw,
	Flyweight, Gadgeteer, GoodFootwork, HardHitter, Workhorse,
	JackOfAllTrade, LightEater, Loner, NeverSurrender, PackMule,
	PersonalArithmetic, Sharpshooter, Sleepless, Stoic, Therapist,
	WeaponSpecialist, ZoneCook,
};
enum mutation_s : unsigned char {
	AcidSpit, Amphibian, CorpseEater, ExtreamReflexes, FlameBreather,
	FourArmed, FrogLegs, HumanMagnet, HumanPlant, Insectoid,
	InsectWings, Luminescence, Manbeast, MindTerror, Puppeteer,
	Parasite, Pathokinesis, Pyrokinesis, Reptilian, RatEater,
	Sonar, Spores, Sprinter, Telepathy, Tracker,
};
enum range_s : unsigned char {
	ArmsLenght,
};
enum tag_s : unsigned char {
	Scrap, Magasine, Clip, Autofire, Light, Heavy,
};
typedef short unsigned	indext;
typedef dataset<attribute_s, Empathy> attributea;
typedef dataset<skill_s, Intimidate> skilla;
typedef cflags<tag_s>	taga;
typedef cflags<mutation_s> mutaniona;
const indext			Blocked = 0xFFFF;
struct attributei {
	const char*			id;
};
struct itemi {
	const char*			id;
	taga				tags;
	char				quality;
	char				damage;
};
struct rolei {
	const char*			id;
	attribute_s			attribute;
	skill_s				skill;
};
struct skilli {
	const char*			id;
	attribute_s			attribute;
};
class item {
	item_s				type;
	union {
		unsigned char	count;
		struct {
			int			quility : 3;
			bool		modified : 1;
		};
	};
public:
	constexpr item() : type(NoItem), count(0) {}
};
class nameable {
	short unsigned		name;
public:
	variant				kind;
	constexpr nameable(variant_s k = Role) : kind(k, 0), name(0) {}
	void				actv(stringbuilder& sb, const char* format, const char* format_param);
	static short unsigned choose_name(role_s role, gender_s gender, bool interactive);
	gender_s			getgender() const;
	const char*			getname() const;
	void				setname(short unsigned v) { name = v; }
};
class statable {
	attributea			attributes;
	skilla				skills;
	mutaniona			mutation;
public:
	void				create(role_s v);
	constexpr int		get(attribute_s i) const { return attributes.get(i); }
	constexpr int		get(skill_s i) const { return skills.get(i); }
	int					getattributepoints() const;
	int					getskillpoints() const;
	constexpr bool		is(mutation_s v) const { return mutation.is(v); }
	constexpr void		set(attribute_s i, int v) { attributes.set(i, v); }
	constexpr void		set(skill_s i, int v) { skills.set(i, v); }
};
class character : public nameable, public statable {
public:
	constexpr character() : nameable(), statable() {}
	void				clear();
	static void			create_new();
	int					getmaximum(attribute_s v) const;
	int					getminimum(skill_s v) const;
	role_s				getrole() const;
};
class worldmap : public datamap<25 * 20> {
public:
	static constexpr int xmax = 25;
	static constexpr int ymax = 20;
	static indext		geti(int x, int y) { return y * xmax + x; }
	static point		getp(int x, int y);
	static int			getx(indext i) { return i % xmax; }
	static int			gety(indext i) { return i / xmax; }
	void				explore(indext i);
	void				fow();
	void				paint();
	void				set(indext i, int v);
	void				set(indext i, int r, int v, bool check);
};
struct locationi {
	const char*			id;
	posable				world;
	explicit constexpr operator bool() const { return id != 0; }
	int					getvelocity() const { return 50; }
	void				paint() const;
};
class gamei {
	static constexpr unsigned rounds_per_day = 24 * 4;
	worldmap			world;
	moveable			party;
	unsigned			rounds;
	static void			render_world();
	static void			update_moving();
	static void			worldmap_input();
public:
	static indext		geti(point v) { return worldmap::geti(v.x / gui.grid, v.y / gui.grid); }
	static point		getp(indext v);
	static character*	getplayer();
	static void			playworld();
	static void			main_menu();
	void				setexplored(indext i) { setexplored(i, 1); }
	void				setexplored(indext i, int r);
	void				setmovement(point v) { party.setmovement(v, rounds); }
	void				setposition(point v) { party.setposition(v); }
};
extern gamei			game;

VKIND(attribute_s, Attribute)
VKIND(gender_s, Gender)
VKIND(role_s, Role)

VKIND(locationi, Location)