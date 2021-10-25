#include "answers.h"
#include "calendar.h"
#include "dataset.h"
#include "event.h"
#include "flagable.h"
#include "point.h"
#include "varianta.h"

enum action_s : unsigned char {
    BuildProvince, DestroyProvince, BuildCapital,
};
enum stat_s : unsigned char {
    Attack, Defend, Raid, Move, Damage, Shield, Hits, Level,
    Explored, Population, PopulationGrowth, Rebellion, Happiness
};
enum cost_s : unsigned char {
    Gold, Mana, Artefacts, Faith, Fame
};
enum prefix_s : unsigned char {
    Minus, Permanent,
};
enum variant_s : unsigned char {
    NoVariant,
    Bonus, Building, Cost, Event, Hero, Landscape, Nation, Player, Prefix, Province, Resource, Stat, Tactic, Troop, Unit
};
struct playeri;
struct prefixa : flagable<4> {
};
struct buildinga : flagable<8> {
};
struct resourcea : flagable<2> {
};
struct producea : adat<char, 12> {
};
struct landscapea : flagable<2> {
};
struct tactica : flagable<4> {
};
struct stata : dataset<Happiness, short> {
};
struct nameable {
    const char* id;
};
struct actioni {
    const char* id;
};
struct nationi : nameable {
    int         alignment;
    const char* avatar;
};
struct range {
    int         min, max;
    explicit constexpr operator bool() const { return max != 0; }
    int         random() const { return xrand(min, max); }
};
struct costa : dataset<8, short> {
    void        apply(variant v, const prefixa& flags);
};
struct bonusi {
    const char* id;
    variant     type;
    int         bonus;
    bool        isrequired() const;
};
struct landscapei {
    const char* id;
    const char* image;
    range       explored, population;
    costa       income; // Base income from landscape
    void        getinfo(stringbuilder& sb) const;
};
struct stati {
    const char* id;
};
struct costi {
    const char* id;
    bool        visible;
};
struct resourcei : nameable {
};
struct populationi {
    const char* id;
    int         population;
    int         level;
    range       getrange() const;
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
    uniti*      encounter_tought[4];
    uniti*      encounter_monster[4];
    int         get(variant v) const;
};
struct army : adat<variant, 18> {
    bool        conquer(army& enemy, stringbuilder* psb, stat_s attacker_stat, stat_s defender_stat);
    void        damage(int hits, stringbuilder* sb = 0);
    int         get(variant v, stringbuilder* sb = 0) const;
};
struct buildingi : nameable {
    buildingi*  base;
    variants    effect;
    variant     condition;
    void        apply(stata& stats, costa& cost) const;
    void        getinfo(stringbuilder& sb) const;
};
struct provincei : nameable {
    uniti*      dwellers;
    landscapei* landscape;
    point       position;
    stata       stats; // Common province and unit stats
    costa       income; // Additional income
    resourcea   resources; // Known province resource
    variant     garnison; // Contract on province garnison units.
    variants    neightboards;
    playeri*    owner;
    buildinga   buildings;
    bool        build(const buildingi* p, bool run);
    void        canbuild(answers& an);
    void        initialize();
    bool        isbuilded(const buildingi* p) const;
    bool        ismatch(variant v) const;
    bool        isupgraded(const buildingi* p) const;
    void        generate_explored();
    void        generate_population();
    void        generate_units();
    int         get(stat_s v) const { return stats.get(v); }
    void        getbuildings(answers& an);
    int         getbuildcount() const;
    void        paint() const;
    void        set(stat_s i, int v) { stats.set(i, v); }
    void        update();
};
struct hero : uniti {
    const char* avatar;
    provincei*  province;
    int         golds;
};
struct troop {
    unsigned    uid;
    uniti*      type;
    provincei*  province;
    provincei*  moveto;
    constexpr explicit operator bool() const { return uid != 0; }
    void        clear();
    int         get(variant id, stringbuilder* sb = 0) const;
    static int  getbonus(variant id, const variants& source);
    void        kill();
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
    int         get(variant v) const;
    void        initialize();
};
struct prefixi {
    const char* id;
};
struct selector : varianta {
    void        match(const landscapei* p, bool keep);
    void        querry(const provincei* p);
    void        sortunits();
};
class gamei {
    unsigned char uid_base;
    unsigned    uid;
    int         turn;
    int         start_year;
public:
    playeri*    player;
    provincei*  province;
    buildingi*  building;
    void        addtroop(uniti* type, provincei* province);
    unsigned    adduid();
    bool        apply(const variants& source, bool allow_test, bool allow_apply);
    static void apply(variant v, stata& stat, costa& cost);
    static void build();
    static const buildingi* choose_building();
    static provincei* choose_province();
    static void choose_province_action();
    static void format(stringbuilder& sb, const char* string, ...);
    static int  get(variant v, const variants& source);
    static variant getaction(variant v);
    static int  getbonus(variant v);
    void        getdate(stringbuilder& sb) const;
    static void getinfo(stringbuilder& sb, const char* id);
    static void getinfov(stringbuilder& sb, variant v);
    int         getmonth() const { return (turn / 3) % 12; }
    int         getmonthpart() const { return turn % 3; }
    int         getyear() const { return start_year + turn / (3 * 12); }
    int         getturn() const { return turn; }
    void        initialize();
    void        passturn();
    void        play(const eventi* event);
    static void playerturn();
    void        setuidbase(unsigned char v) { uid_base = v; }
};
extern gamei    game;
namespace draw {
long            dialog(answers& an, const char* title, const char* format);
void            initialize();
void            maketurn();
}
inline bool     chance(int value) { return (rand() % 100) < value; }
int             getfix(stringbuilder* sb, int v, variant id);
VKIND(bonusi, Bonus)
VKIND(buildingi, Building)
VKIND(cost_s, Cost)
VKIND(eventi, Event)
VKIND(landscapei, Landscape)
VKIND(provincei, Province)
VKIND(stat_s, Stat)
VKIND(tactici, Tactic)
VKIND(troop, Troop)
VKIND(uniti, Unit)