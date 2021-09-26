#include "crt.h"
#include "flagable.h"
#include "gender.h"
#include "menu.h"
#include "posable.h"
#include "variant.h"

enum duration_s : unsigned char {
	Instant, Round, PerUse, Infinite,
};
enum condition_s : unsigned char {
	AllyNearTarget, NoAllyNearTarget, EnemyNearTarget,
	YouIsInvisible,
};
enum card_s : unsigned char {
	StandartCard, Discard, Exhause,
};
enum area_s : unsigned char {
	NoArea, Slash, Circle, Ray, Splash, Spray,
};
enum feat_s : unsigned char {
	EnemyAttackYouInsteadNearestAlly, GainExpForRetaliate,
	TargetAllAdjancedEnemies, TargetEnemyMoveThrought,
	Jump, Fly,
};
enum statistic_s : unsigned char {
	Moved, Attacked, Coins,
	MonstersKilled, ItemsUsed,
};
enum action_s : unsigned char {
	Shield, Retaliate, Guard, Evasion,
	Move, Attack, Push, Pull, Heal, DisarmTrap, SetTrap, Summon, Loot,
	Range, Target, Pierce,
	Bless, Curse,
	Experience, Level, Use,
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
	Action, ActionBonus, Area, Card, CardType, Duration, Element, Feat, Menu, Player, State,
};
typedef flagable<2> statef;
typedef flagable<1> elementf;
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
};
struct statei {
	const char*			id;
};
class statable {
	statef				states;
	char				actions[Use + 1];
public:
	int					get(variant v) const;
	void				set(variant i, int v);
};
class creature : public posable, public statable {
	statable			basic;
	variant				kind;
	variant				owner;
};