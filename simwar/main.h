#include "answers.h"
#include "calendar.h"
#include "dataset.h"
#include "event.h"
#include "flagable.h"
#include "point.h"
#include "varianta.h"

enum action_s : unsigned char {
    BuildCapital, BuildProvince, ImproveDefence,
};
enum action_flag_s : unsigned char {
    UseOnProvince, UseOnPlayer, UseOnHero
};
enum stat_s : unsigned char {
    Attack, Defend, Raid, Move, Damage, Hits, Level,
    Explored, Population, Growth, Rebellion, Unrest
};
enum cost_s : unsigned char {
    Gold, Mana, Artefacts, Faith, Fame
};
enum prefix_s : unsigned char {
    Minus, Permanent,
};
enum variant_s : unsigned char {
    NoVariant,
    Action, Bonus, Cost, Event, Hero, Landscape, Nation, Player, Prefix, Province, Resource, Stat, Troop, Unit
};
struct playeri;
struct prefixa : flagable<4> {
};
struct resourcea : flagable<2> {
};
struct producea : adat<char, 12> {
};
struct landscapea : flagable<2> {
};
struct stata : dataset<Unrest, short> {
};
struct nameable {
    const char* id;
};
struct nationi : nameable {
    int         alignment;
};
struct range {
    int         min, max;
    explicit constexpr operator bool() const { return max != 0; }
    int         random() const { return xrand(min, max); }
};
struct costa : dataset<8, short> {
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
struct uniti : nameable {
    nationi*    nation;
    stata       stats;
    costa       cost, upkeep;
    landscapea  encounter;
    producea    need;
    const char* avatar;
    uniti*      encounter_tought[4];
    uniti*      encounter_monster[4];
    int         get(variant v) const;
};
struct army : adat<variant, 18> {
    bool        conquer(army& enemy, stringbuilder* psb, stat_s attacker_stat, stat_s defender_stat);
    void        damage(int hits, stringbuilder* sb = 0);
    int         get(variant v, stringbuilder* sb = 0) const;
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
    void        generate_explored();
    void        generate_population();
    void        generate_units();
    int         get(stat_s v) const { return stats.get(v); }
    void        initialize();
    void        paint() const;
    void        set(stat_s i, int v) { stats.set(i, v); }
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
struct actioni {
    const char* id;
    unsigned    flags;
    bool        is(action_flag_s v) const { return (flags & FG(v)) != 0; }
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
    void        addtroop(uniti* type, provincei* province);
    unsigned    adduid();
    bool        apply(const variants& source, bool allow_test, bool allow_apply);
    static void format(stringbuilder& sb, const char* string, ...);
    static int  get(variant v, const variants& source);
    static variant getaction(variant v);
    static int  getbonus(variant v);
    void        getdate(stringbuilder& sb) const;
    int         getmonth() const { return (turn / 3) % 12; }
    int         getmonthpart() const { return turn % 3; }
    int         getyear() const { return start_year + turn / (3 * 12); }
    int         getturn() const { return turn; }
    void        initialize();
    void        passturn();
    void        play(const eventi* event);
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
VKIND(actioni, Action)
VKIND(action_s, Action)
VKIND(bonusi, Bonus)
VKIND(cost_s, Cost)
VKIND(eventi, Event)
VKIND(stat_s, Stat)
VKIND(provincei, Province)
VKIND(troop, Troop)
VKIND(uniti, Unit)