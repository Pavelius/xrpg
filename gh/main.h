#include "crt.h"
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
	Jump, Fly,
};
enum statistic_s : unsigned char {
	Moved, Attacked, Coins,
	MonstersKilled, ItemsUsed,
};
enum action_s : unsigned char {
	Shield, Retaliate, Evasion,
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
	Ally, Enemy,
};
enum variant_s : unsigned char {
	NoVariant,
	Action, ActionBonus, Area, Card, CardType, CombatCard, Duration, Element, Feat,
	Menu, Monster, Player, State, SummonedCreature, Trap, Type
};
typedef flagable<2> statef;
typedef flagable<1> elementf;
typedef varianta combatdeck;
struct actioni {
	const char*			id;
	int					type;
	bool				ismain() const;
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
struct feati {
	const char*			id;
};
struct playeri {
	const char*			id;
	short				health[10];
	combatdeck			combat;
	adat<variant, 32>	actions;
	void				buildcombatdeck();
};
struct statei {
	const char*			id;
};
class statable {
	statef				states;
	char				actions[Level + 1];
public:
	int					get(variant v) const;
	void				set(variant i, int v);
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
class creature : public posable, public statable {
	statable			basic;
	variant				kind;
	variant				owner;
};