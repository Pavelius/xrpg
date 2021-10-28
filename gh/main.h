#include "answers.h"
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
	TargetEnemyMoveThrought
};
enum statistic_s : unsigned char {
	Moved, Attacked, Looted,
	MonstersKilled, ItemsUsed,
};
enum action_s : unsigned char {
	Shield, Retaliate,
	Move, Attack, Push, Pull, Heal, DisarmTrap, Loot,
	Range, Target, Pierce,
	Bless, Curse, RecoverDiscarded,
	Experience,
	Level,
	Pay, ChooseGoldOrReputation,
	Shuffle, NextAction
};
enum action_bonus_s : char {
	InfiniteCount = 100, MovedCount, AttackedCount, ShieldCount,
};
enum state_s : unsigned char {
	Disarmed, Immobilize, Wound, Muddle, Poison, Invisibility, Stun, Strenght,
	Jump, Fly, Mirrored, Hostile, UseUpper, UseLower, Elite,
};
enum element_s : unsigned char {
	Fire, Ice, Air, Earth, Light, Dark, AnyElement,
};
enum variant_s : unsigned char {
	NoVariant,
	Action, ActionBonus, Area, Card, CardType, City, CombatCard, Duration, Element,
	Feat, GameProperty, Menu, Monster, MonsterCard, Object, Player,
	Scenario, State, SummonedCreature, Tile, Trap, Type
};
enum game_propery_s : unsigned char {
	Reputation, Prosperty, Coins, Donate,
};
enum stage_s : unsigned char {
	CardSelections, DetermineInitiative, CreaturesTurns,
};
typedef short unsigned indext;
typedef flagable<2> statef;
typedef flagable<1 + TargetEnemyMoveThrought / 8> featf;
typedef flagable<1> elementf;
typedef flagable<4>	playerf;
const indext Blocked = 0xFFFF;
struct deck : varianta {
	void				addcombat(variant owner);
	void				addcombat8(const char* abilities);
	void				changeone();
	void				drop(variant v) { add(v); }
	variant				get();
	variant				look(int n = 0) const;
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
	void				getinfo(stringbuilder& sb) const;
};
struct cardi {
	const char*			id;
	char				level;
	variant				owner;
	int					initiative;
	variants			upper, lower;
	void				getinfo(stringbuilder& sb) const;
};
struct monstercardi {
	const char*			id;
	int					initiative;
	variants			abilities;
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
	gender_s			gender;
	char				level, exp, coins;
	variant				cards[2];
	short				health[10];
	deck				combat, hand, discard, lost;
	int					get(action_s v) const;
	int					getinitiative() const;
	void				getinfo(stringbuilder& sb) const;
	void				buildcombatdeck();
	void				buildhand();
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
	const char*			avatar;
};
struct summoni {
	const char*			id;
	char				hits, move, attack, range;
	variants			feats;
};
struct abilityi {
	char				hits, move, attack, range;
	variants			feats;
	int					get(action_s v) const;
};
struct monsteri {
	const char*			id;
	const char*			abilities;
	deck				abilities_deck;
	abilityi			normal[8], elite[8];
	void				buildcombatdeck();
	const abilityi&		get(int level, bool tought) const;
	int					getinitiative() const;
	void				getinfo(stringbuilder& sb) const;
};
struct nameable {
	variant				kind;
	void                act(const char* format, ...) const;
	gender_s			getgender() const;
	const char*         getname() const { return kind.getname(); }
};
class object : public posable, public nameable {
	short               hits;
	char				level, initiative;
	statef              states;
	void				paint_creature() const;
	void				paint_default() const;
	void				paint_tile() const;
public:
	constexpr operator bool() const { return hits != 0; }
	void				activate(duration_s duration, int count, variant source, variants variants);
	void				addattack(scripti& parameters) const;
	void				apply(const variants& source);
	void				apply(scripti& parameters);
	void                applyaction(variant v);
	void				action(action_s a, bool interactive, bool hostile, int range, int targets, int bonus);
	void				attack(int damage, int range, int pierce, int targets, statef additional);
	void                attack(object& enemy, const scripti& parameters);
	void				beginround();
	variant				chooseaction() const;
	const variants&     choosepart(const cardi& e);
	void                clear();
	void				create(variant v);
	void				damage(int value);
	void				endround();
	void				focusing() const;
	int                 get(variant v) const;
	const char*         getavatar() const { return kind.getid(); }
	void				getinfo(stringbuilder& sb) const;
	indext				getindex() const;
	int					getinitiative() const;
	object*				getnearestenemy() const;
	deck&				getcombatdeck() const;
	int                 getmaximumhits() const;
	int                 getpriority() const;
	void				heal(int v);
	bool                is(state_s v) const { return states.is(v); }
	bool				isalive() const;
	bool                isenemy(const object& e) const { return is(Hostile) != e.is(Hostile); }
	bool				isfocused() const;
	bool                isinteractive() const { return isplayer(); }
	bool                isplayer() const { return kind.type==Player; }
	bool				isusable() const;
	void                kill();
	void                loot(int v);
	void                maketurn();
	void				modify(scripti& e) const;
	void				move(int v);
	void                movefrom(indext i, int range);
	void                moveto(indext i, int range);
	void				moveto(indext i, int range, int distance);
	void				moving(indext i, bool interactive);
	void				paint() const;
	void				refresh();
	void                remove(state_s v) { states.remove(v); }
	void                set(variant i, int v);
	void				use(action_s v);
};
struct objects : adat<object*, 64> {
    object*             choose() const;
	void                match(variant v, bool keep);
	void                matchrange(int range, bool keep);
	void                paint(bool allow_hilite, bool allow_drag) const;
    void                select();
    void                selectalive();
	void                selectground();
	void                sort();
	void				sortnearest();
};
struct activity {
	variant				owner, source;
	variants			effect;
	duration_s			duration;
	char				count;
	constexpr operator bool() const { return owner; }
};
const int				hms = 32;
class mapi {
	flagable<hms*hms>	walls;
public:
	void				clearpath();
	void				block(const objects& vs);
	void				block(indext i) { setmove(i, Blocked); }
	void                blockrange(int range);
	void				blockwalls();
	indext              getfarest(const short unsigned* source);
	indext              getnearest(const short unsigned* source);
	short unsigned		getmove(indext i) const;
	void                getmove(short unsigned* destination);
	bool				iswall(indext i) const { return walls.is(i); }
	void				makewave(indext start_index);
	void				setmove(indext i, short unsigned v);
	void                setmove(const short unsigned* source);
	void				setpass(indext i) { walls.remove(i); }
	void				setwall(indext i) { walls.set(i); }
	indext				to(indext i, int d) const;
};
struct scenariotiles {
	variant				tile;
	point				position;
	bool				inverse;
	constexpr operator bool() const { return tile.operator bool(); }
};
struct scenarioi {
	const char*			id;
	point				starts[8];
	scenariotiles		tiles[8];
	point				getstart(int index) const { return starts[index]; }
	void				prepare(int stage);
};
struct gamei : public mapi {
	short				ability[Donate + 1];
	int					difficult, rounds;
	stage_s				stage;
	adat<variant, 4>	players;
	playerf				allowed_players;
	deck				market;
	deck				road, city;
	deck				enemy_combat;
	char				elements[AnyElement];
	void				add(variant i, int v = 1);
	void				beginround();
	void				buildcombatdeck();
	object*				create(variant v, bool inverse);
	object*				create(point position, variant v, bool inverse);
	void				endround();
	bool				isallow(variant v) const;
	int					get(variant i) const;
	static int			getinitiative(variant v);
	int					getlevel() const;
	void				makeround();
	static void			maketurn();
	static void			playtactic();
	static void			selectkind(varianta& source);
	static void			sort(varianta& source);
	void				set(variant i, int v);
};
struct tilei {
	const char*			id;
	point				size; // size of field in tiles
	int					priority;
	point				offset; // offset to upper left tile
	slice<point>		blocks; // blocked squares
	void				creating(point position, bool inverse) const;
};
struct eventi {
	struct action {
		int				stage;
		const char*		text;
		variants		consequences;
	};
	const char*			text;
	const char*			case1;
	const char*			case2;
	action				actions[6];
	void				clear();
	static bool			read(const char* url, array& source);
};
extern array			city_events, road_events;
extern gamei			game;
extern cardi*			last_card;
extern object*          last_object;
inline point			i2h(indext i) { return {(short)(i % hms), (short)(i / hms)}; }
inline indext			h2i(point v) { return v.y * hms + v.x; }
point					h2p(point v);
point					p2h(point v);
namespace draw {
void					centercamera(point v);
indext					choosemovement();
extern const object*	focused_object;
void					initializex();
void					moving(point& result, point pt, int step);
void					status(const char* format, ...);
void					tooltips(const char* format, ...);
}
VKIND(action_s, Action)
VKIND(area_s, Area)
VKIND(cardi, Card)
VKIND(duration_s, Duration)
VKIND(element_s, Element)
VKIND(feat_s, Feat)
VKIND(game_propery_s, GameProperty)
VKIND(monsteri, Monster)
VKIND(monstercardi, MonsterCard)
VKIND(object, Object)
VKIND(playeri, Player)
VKIND(state_s, State)
VKIND(variant_s, Type)