#include "crt.h"
#include "draw_hexagon.h"
#include "flagable.h"
#include "gender.h"
#include "menu.h"
#include "posable.h"
#include "variant.h"
#include "varianta.h"

enum duration_s : unsigned char {
	Instant, Round, PerUse, Infinite,
};
enum card_s : unsigned char {
	StandartCard, Discard, Exhause,
};
enum area_s : unsigned char {
	NoArea, Slash, Circle, Ray, Splash, Spray,
};
enum feat_s : unsigned char {
	EnemyAttackYouInsteadNearestAlly, GainExpForRetaliate, GainExpForTarget,
	TargetAllAlly, TargetAllAdjancedEnemies, TargetEnemyMoveThrought,
	Jump, Fly, NextAction,
};
enum statistic_s : unsigned char {
	Moved, Attacked, Coins,
	MonstersKilled, ItemsUsed,
};
enum action_s : unsigned char {
	Shield, Retaliate,
	Move, Attack, Push, Pull, Heal, DisarmTrap, Loot,
	Range, Target, Pierce,
	Bless, Curse, RecovedDiscarded,
	Experience, Level
};
enum action_bonus_s : char {
	InfiniteCount = 100, MovedCount, AttackedCount, ShieldCount,
};
enum state_s : unsigned char {
	Disarmed, Immobilize, Wound, Muddle, Poison, Invisibility, Stun, Strenght,
};
enum element_s : unsigned char {
	Fire, Ice, Air, Earth, Light, Dark, AnyElement,
};
enum fraction_s : unsigned char {
	Neutral, Ally, Enemy,
};
enum variant_s : unsigned char {
	NoVariant,
	Action, ActionBonus, Area, Card, CardType, CombatCard, Duration, Element,
	Feat, Fraction, GameProperty, Menu, Monster, Object, Player,
	Scenario, State, SummonedCreature, Tile, Trap, Type
};
enum game_propery_s : unsigned char {
	Reputation, Prosperty, Donate,
};
typedef short unsigned indext;
typedef flagable<2> statef;
typedef flagable<1 + NextAction / 8> featf;
typedef flagable<1> elementf;
typedef flagable<4>	playerf;
const indext Blocked = 0xFFFF;
struct deck : varianta {
	void				addcombat(variant owner);
	void				drop(variant v) { add(v); }
	variant				get();
	variant				look(int n = 0);
	void				shuffle();
};
struct actioni {
	const char*			id;
};
struct areai {
	const char*			id;
};
struct actionbonusi {
	const char*			id;
	variant				action;
	int					bonus;
};
struct cardi {
	const char*			id;
	char				level;
	variant				owner;
	int					initiative;
	variants			upper, lower;
	void				getinfo(stringbuilder& sb) const;
};
struct combatcardi {
	const char*			id;
	variant				owner;
	char				count, bonus;
	variants			feats;
};
struct cardtypei {
	const char*			id;
};
struct durationi {
	const char*			id;
};
struct elementi {
	const char*			id;
};
struct fractioni {
	const char*			id;
};
struct gamepropertyi {
	const char*			id;
};
struct feati {
	const char*			id;
};
struct playeri {
	const char*			id;
	gender_s			gender;
	short				health[10];
	deck				combat, hand, active, discard, lost;
	void				buildcombatdeck();
};
struct statei {
	const char*			id;
	bool				hostile;
};
struct scripti {
	variant				action;
	char				bonus, pierce, range, targets, experience;
	area_s				area;
	char				area_size;
	duration_s			duration;
	char				duration_lenght;
	elementf			elements_use, elements;
	statef				states;
	featf				feats;
	void				apply(const variants& source) { apply(source.begin(), source.end()); }
	const variant*		apply(const variant * p, const variant * pe);
	static variant		getaction(variant v);
	static int			getbonus(variant v);
	void				clear();
};
struct trapi {
	const char*			id;
	char				damage;
	variants			feats;
};
struct summoni {
	const char*			id;
	char				hits, move, attack, range;
	variants			feats;
};
struct abilityi {
	char				hits, move, attack, range;
	variants			feats;
};
struct monsteri {
	const char*			id;
	const char*			combat;
	abilityi			normal[8], elite[8];
};
struct nameable {
	variant				kind;
	void                act(const char* format, ...) const;
	gender_s			getgender() const;
	const char*         getname() const { return kind.getname(); }
};
class object : public posable, public nameable {
	fraction_s          fraction;
	short               hits;
	statef              states;
public:
	constexpr operator bool() const { return hits != 0; }
	void				activate(duration_s duration, int count, variant source, variants variants);
	void				addattack(scripti& parameters) const;
	void				apply(variant v, int bonus);
	void				attack(int damage, int range, int pierce, int targets, statef additional);
	void                attack(object& enemy, const scripti& parameters);
	void                clear();
	void				create(variant v, fraction_s fraction);
	void				damage(int value);
	int                 get(variant v) const;
	deck&				getcombatdeck() const;
	fraction_s          getfraction() const;
	int                 getmaximumhits() const;
	void				heal(int v);
	bool                is(state_s v) const { return states.is(v); }
	bool                isinteractive() const { return true; }
	bool                ishostile() const;
	bool                isplayer() const { return false; }
	void                kill();
	bool                match(variant v) const;
	void				move(int v);
	void				paint() const;
	void				pull(int range, int targets);
	void				push(int range, int targets);
	void                remove(state_s v) { states.remove(v); }
	void                set(variant i, int v);
	void				use(action_s v);
};
struct activity {
	variant				owner, source;
	variants			effect;
	duration_s			duration;
	char				count;
	constexpr operator bool() const { return owner; }
};
const int				hms = 64;
inline point			i2h(indext i) { return {(short)(i % hms), (short)(i / hms)}; }
inline indext			h2i(point v) { return v.y * hms + v.x; }
class mapi {
	flagable<hms*hms>	walls;
public:
	bool				iswall(indext i) const { return walls.is(i); }
	void				setpass(indext i) { walls.remove(i); }
	void				setwall(indext i) { walls.set(i); }
};
struct scenariotiles {
	variant				tile;
	point				position;
	constexpr operator bool() const { return tile.operator bool(); }
};
struct scenarioi {
	const char*			id;
	scenariotiles		tiles[8];
};
struct gamei : public mapi {
	short				ability[Donate + 1];
	adat<variant, 4>	players;
	playerf				allowed_players;
	deck				market;
	deck				road, city;
	deck				enemy_combat;
	char				elements[AnyElement];
	void				add(variant i, int v = 1);
	void				buildcombatdeck();
	object*				create(variant v, fraction_s fraction);
	bool				isallow(variant v) const;
	int					get(variant i) const;
	void				set(variant i, int v);
};
struct collection : varianta {
	void                match(variant v, bool keep);
};
struct tilei {
	const char*			id;
	point				size; // size of field in tiles
	point				offset; // offset to upper left tile
	sliceu<point>		blocks; // blocked squares
};
extern gamei			game;
namespace draw {
void					initializex();
void					status(const char* format, ...);
void					tooltips(const char* format, ...);
}
VKIND(action_s, Action)
VKIND(area_s, Area)
VKIND(duration_s, Duration)
VKIND(feat_s, Feat)
VKIND(fraction_s, Fraction)
VKIND(state_s, State)