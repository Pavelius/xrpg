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
	Jump, Fly, NextAction,
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
	GameProperty, Menu, Monster, Player, State, SummonedCreature, Trap, Type
};
enum game_propery_s : unsigned char {
	Reputation, Prosperty, Donate,
};
typedef flagable<2> statef;
typedef flagable<1 + NextAction / 8> featf;
typedef flagable<1> elementf;
typedef flagable<4>	playerf;
struct deck : varianta {
	void				addcombat(variant owner);
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
struct gamepropertyi {
	const char*			id;
};
struct feati {
	const char*			id;
};
struct playeri {
	const char*			id;
	short				health[10];
	deck				combat, hand, active, discard, lost;
	void				buildcombatdeck();
};
struct statei {
	const char*			id;
	bool				hostile;
};
class statable {
	statef				states;
	featf				feats;
	char				actions[Level + 1];
public:
	void				apply(const variants& source);
	void				damage(int value);
	int					get(variant v) const;
	statef				getstates() const { return states; }
	static variant		getaction(variant v);
	static int			getbonus(variant v);
	const variant*      parse(const variant* p, const variant* pe);
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
class creature : public posable {
	variant				kind;
	char                hits;
	statef              states;
public:
	void                act(const char* format, ...) const;
	void				apply(variant v, int bonus);
	void				attack(int damage, int range, int pierce, statef additional);
	void                attack(creature& enemy, int damage, int pierce, statef additional);
	int                 getmaximumhits() const { return 0; }
	const char*         getname() const { return kind.getname(); }
	void				heal(int v);
	bool                isinteractive() const { return true; }
	bool                is(state_s v) const { return states.is(v); }
	void				move(int v);
	void				pull(int range, int targets);
	void				push(int range, int targets);
	void                remove(state_s v) { states.remove(v); }
	void                setkind(variant v) { kind = v; }
	deck&				getcombatdeck() const;
};
struct gamei {
	short				ability[Donate + 1];
	adat<variant, 4>	players;
	playerf				allowed_players;
	deck				market;
	deck				road, city;
	deck				enemy_combat;
	void				add(variant i, int v = 1);
	void				buildcombatdeck();
	bool				isallow(variant v) const;
	int					get(variant i) const;
	void				set(variant i, int v);
};
extern gamei			game;
namespace draw {
    void                status(const char* format, ...);
}
VKIND(action_s, Action)
VKIND(feat_s, Feat)