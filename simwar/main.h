#include "answers.h"
#include "dataset.h"
#include "flagable.h"
#include "variant.h"

enum stat_s : unsigned char {
    Attack, Defend, Raid, Move, Damage, Hits,
    Level,
    Explored, Population, Growth, Rebellion, Unrest
};
enum cost_s : unsigned char {
    Gold, Mana, Fame
};
enum prefix_s : unsigned char {
    Minus, Income,
};
enum variant_s : unsigned char {
    NoVariant, Bonus, Cost, Landscape, Player, Prefix, Resource, Stat, Troop, Unit
};
enum labdscape_s : unsigned char {
    Plains, Forest, Hills, Swamp, Desert, Waste, Sea,
};
struct bonusi {
    const char* id;
    variant     type;
    int         bonus;
};
struct costa : dataset<8, short> {
};
struct nameable {
    const char* id;
};
struct stati {
    const char* id;
};
struct costi {
    const char* id;
};
struct resourcei {
    const char* id;
};
struct prefixa : flagable<2> {
};
struct resourcea : flagable<2> {
};
struct stata : dataset<Unrest> {
};
struct uniti : nameable {
    variants    stats;
    costa       cost, upkeep;
};
struct troop {
    variant     type;
    variant     owner;
    void        clear();
    int         get(variant id, stringbuilder* sb = 0) const;
    static int  getbonus(variant id, const variants& source);
    void        kill();
};
struct army : adat<variant, 18> {
    bool        conquer(army& enemy, stringbuilder* psb, stat_s attacker_stat, stat_s defender_stat);
    void        damage(int hits, stringbuilder* sb = 0);
    int         get(variant v, stringbuilder* sb = 0) const;
};
struct hero : uniti {
    army        troops;
};
struct landscapei : nameable {
    costa       income; // Base income from landscape
};
struct landscapea : flagable<2> {
};
struct provincei : nameable {
    costa       income; // Additional income
    resourcea   resources; // Known province resource
    variant     garnsison; // Contract on province garnison units.
    stata       stats; // Common province and unit stats
    void        clear();
};
struct sitetypei : nameable {
    landscapea  landscape; // Landscape types where site might generate. 0 - for all sites.
    variants    feats;
    variant     dialog;
};
struct sitei {
    variant     type;
    variant     owner;
    char        conceal; // Percent chance of conceal site. Visible only if lower that `explored` province.
};
struct eventi {
    const char* id;
    constexpr explicit operator bool() const { return id != 0; }
    static const eventi* find(const char* id);
    void        play() const;
    static void read(const char* url);
};
struct eventcasei {
    short       parent;
    short       id;
    int         next;
    const char* text;
    variants    effect;
    constexpr explicit operator bool() const { return parent != -1; }
    void        clear();
    eventi*     getparent() const;
    bool        isallow() const;
    bool        isprompt() const { return next == -1; }
    bool        isend() const { return !next; }
};
struct playeri {
    const char* id;
    costa       total;
    int         get(variant v) const;
    static playeri* getcurrent();
};
struct prefixi {
    const char* id;
};
struct gamei {
    playeri*    player;
    provincei*  province;
    void        apply(const variants& source);
    static variant getaction(variant v);
    static int  getbonus(variant v);
    static void format(stringbuilder& sb, const char* string, ...);
};
extern gamei    game;
namespace draw {
long            dialog(answers& an, const char* title, const char* format);
void            initialize();
}
int             getfix(stringbuilder* sb, int v, variant id);
VKIND(bonusi, Bonus)
VKIND(cost_s, Cost)
VKIND(stat_s, Stat)
VKIND(troop, Troop)
VKIND(uniti, Unit)