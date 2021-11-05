#include "answers.h"
#include "calendar.h"
#include "dataset.h"
#include "event.h"
#include "flagable.h"
#include "point.h"
#include "pushvalue.h"
#include "crange.h"
#include "varianta.h"

enum tag_s : unsigned char {
	Offensive, Defensive, Shooting, Looting,
	Mobile, Fast, Flying,
	ForestWise, SwampWise, HillsWise,
	Hired, Summoned, Undead,
};
enum action_s : unsigned char {
	CancelAction,
	BuildProvince, DestroyProvince, BuildCapital,
	AttackProvince, RaidProvince,
	RecruitUnits,
	ChooseHeroes, ChooseProvinces, ShowBuildings, ShowSites, EndTurn
};
enum stat_s : unsigned char {
	Level, Damage, Shield, Move, Hits,
	Explored, Population, PopulationGrowth, Rebellion, Happiness
};
enum cost_s : unsigned char {
	Gold, Mana, Artifacts, Faith, Fame
};
enum prefix_s : unsigned char {
	Minus, Permanent,
};
enum variant_s : unsigned char {
	NoVariant,
	Bonus, Building, Cost, Event, Hero, Landscape, Nation, Player, Prefix, Province, Resource, Stat, Tactic, Unit
};
struct playeri;
struct prefixa : flagable<4> {
};
struct buildinga : flagable<8> {
};
struct resourcea : flagable<4> {
	void		getinfo(stringbuilder& sb) const;
};
struct taga : flagable<4> {
	void		getinfo(stringbuilder& sb) const;
};
struct producea : adat<char, 12> {
	void		getinfo(stringbuilder& sb, const char* promt) const;
};
struct landscapea : flagable<2> {
};
struct tactica : flagable<4> {
};
struct stata : dataset<Happiness, short> {
	void		getinfo(stringbuilder& sb, const char* promt) const;
};
struct actiona : dataset<RaidProvince, char> {
};
struct nameable {
	const char* id;
};
struct tagi {
	const char* id;
};
struct actioni {
	const char* id;
	fnevent		proc;
};
struct costa : dataset<8, short> {
	void        apply(variant v, const prefixa& flags);
	void		getinfo(stringbuilder& sb, const char* promt) const;
	void		modify(const resourcea& allowed, const producea& need);
};
struct nationi : nameable {
	int         alignment;
	const char* avatar;
	costa		income;
	stata		stats;
};
struct landscapei {
	const char* id;
	const char* image;
	crange      explored, population;
	costa       income; // Base income from landscape
	void        getinfo(stringbuilder& sb) const;
};
struct stati {
	const char* id;
};
struct costi {
	const char* id;
	bool        visible;
	int			getindex() const { return this - bsdata<costi>::elements; }
};
struct resourcei : nameable {
	costa		cost;
};
struct populationi {
	const char* id;
	int         population;
	int         level;
	crange      getrange() const;
	static const populationi* findbypopulation(int value);
};
struct tactici : nameable {
	variants    bonus;
	tactica     disable;
	void        getinfo(stringbuilder& sb) const;
};
struct uniti : nameable {
	nationi*    nation;
	stata       stats;
	costa       cost, upkeep;
	landscapea  encounter;
	producea    need;
	taga		tags;
	uniti*      encounter_tought[4];
	uniti*      encounter_monster[4];
	int         get(variant v) const;
	void		getinfo(stringbuilder& sb) const;
};
struct troop {
	uniti*		type;
	constexpr explicit operator bool() const { return type != 0; }
	void        clear();
	int			get(variant v) const;
	int         get(variant id, stringbuilder* sb) const;
	static int  getbonus(variant id, const variants& source);
	void        kill();
};
struct army : adat<troop, 18> {
	void		add(uniti* p);
	static bool	choose(const char* title, const char* t1, army& a1, fnevent pr1, const char* t2, army& a2, fnevent pr2);
	bool        conquer(army& enemy, stringbuilder* psb);
	void        damage(int hits, stringbuilder* sb = 0);
	int         get(variant v, stringbuilder* sb = 0) const;
	int			getleadership() const { return 3; }
	int			getstrenght(bool defensive) const;
	int			getunitcount(int rang) const;
	bool		is(tag_s v) const;
	void		normalize();
	void		shuffle();
	void		select(const landscapei* v);
	void		sort();
};
struct defenderi : nameable {
	army		units;
	costa		cost, upkeep;
};
struct oppositioni {
	const defenderi* type;
	char		percent;
	bool		canbecancel() const { return true; }
	void		recover(int bonus = 0);
};
struct buildingi : nameable {
	buildingi*  base;
	variants    effect;
	variant     condition;
	costa       cost;
	void        apply(stata& stats, costa& cost) const;
	void        getinfo(stringbuilder& sb, bool show_cost) const;
	void		getpresent(stringbuilder& sb) const;
};
struct provincei : nameable {
	uniti*      dwellers;
	landscapei* landscape;
	point       position;
	stata       stats; // Common province and unit stats
	costa       income; // Additional income
	resourcea   resources; // Known province resource
	variants    neightboards;
	playeri*    owner;
	buildinga   buildings;
	oppositioni	guard;
	army		garnison;
	bool        build(const buildingi* p, bool run);
	buildingi*	choosebuilding(bool tobuild, bool todemontage);
	bool		demontage(const buildingi* p, bool run);
	void        initialize();
	bool        isbuilded(const buildingi* p) const;
	bool        ismatch(variant v) const;
	bool        isupgraded(const buildingi* p) const;
	void        generate_explored();
	void        generate_population();
	void        generate_units();
	int         get(stat_s v) const { return stats.get(v); }
	int         get(cost_s v) const { return income.get(v); }
	int         getbuildcount() const;
	void		getbuildings(answers* an, stringbuilder* sb);
	int			getbuildlimit() const { return 3; }
	void        getpresent(stringbuilder& sb) const;
	void        paint() const;
	void		refresh();
private:
	void		apply(const buildingi& e, int multiplier = 1);
};
struct heroi : uniti {
	const char* avatar;
	provincei*  province;
	int         golds;
	army		troops;
	void        getinfo(stringbuilder& sb) const;
};
struct sitetypei : nameable {
	landscapea  landscape; // Landscape types where site might generate. 0 - for all sites.
	variants    feats;
	variant     dialog;
};
struct sitei {
	sitetypei*  type;
	provincei*  province;
	char        conceal; // Percent chance of conceal site. Visible only if lower that `explored` province.
};
struct decki : varianta {
	void        addrandom(variant v);
	void        insert(int i, variant v);
};
struct playeri {
	const char* id;
	int         avatar;
	costa       total;
	decki       events;
	actiona		actions; // Total count of each action per turn
	resourcea	resources; // Allowed resources
	int         get(variant v) const;
	bool		isallow(action_s v) const { return actions.get(v) != 0; }
	void        initialize();
	bool		hire(uniti* unit, army* garnison, bool run, stringbuilder* sb = 0);
	void		refresh();
	void		update_resources();
};
struct prefixi {
	const char* id;
};
struct selector : varianta {
	void        match(const landscapei* p, bool keep);
	void        sortunits();
};
class gamei {
	unsigned char uid_base;
	unsigned    uid;
	int         turn;
	int         start_year;
public:
	heroi*		hero;
	playeri*    player;
	provincei*  province;
	buildingi*  building;
	army*		garnison;
	troop*		unit;
	void		addaction(answers& an, action_s v);
	unsigned    adduid();
	bool        apply(const variants& source, bool allow_test, bool allow_apply);
	static void apply(variant v, stata& stat, costa& cost, int multiplier = 1);
	static void build();
	static void demontage();
	static void choose(answers& an, const char* title, const char* header);
	static void	buildings();
	static heroi* choose_hero();
	static provincei* choose_province();
	static void	playermove();
	static void error(stringbuilder* sb, const char* id, ...);
	bool		execute(action_s id, bool run);
	static void format(stringbuilder& sb, const char* string, ...);
	static int  get(variant v, const variants& source);
	void        getdate(stringbuilder& sb) const;
	static void getinfo(stringbuilder& sb, const char* id);
	static int	getleadership(int value, int level);
	int         getmonth() const { return (turn / 3) % 12; }
	int         getmonthpart() const { return turn % 3; }
	int         getyear() const { return start_year + turn / (3 * 12); }
	int         getturn() const { return turn; }
	void        initialize();
	static void	nextmove();
	static void message(const char* string);
	void        passturn();
	void        play(const eventi* event);
	static void	recruit();
	void		refresh();
	void        setuidbase(unsigned char v) { uid_base = v; }
};
extern gamei    game;
namespace draw {
long            dialog(answers& an, const char* title, const char* format);
void			choose(answers& an, const char* title, const char* header);
long			choosel(answers& an, const char* title, const char* header);
void            initialize();
void			setactive(fnevent proc);
void			setlastactive();
}
inline bool     chance(int value) { return (rand() % 100) < value; }
int             getfix(stringbuilder* sb, int v, variant id);
VKIND(bonusi, Bonus)
VKIND(buildingi, Building)
VKIND(cost_s, Cost)
VKIND(eventi, Event)
VKIND(heroi, Hero)
VKIND(landscapei, Landscape)
VKIND(provincei, Province)
VKIND(stat_s, Stat)
VKIND(tactici, Tactic)
VKIND(uniti, Unit)