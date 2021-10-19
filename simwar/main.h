#include "answers.h"
#include "dataset.h"
#include "flagable.h"
#include "variant.h"
#include "varianta.h"

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
    NoVariant, Bonus, Cost, Event, Landscape, Player, Prefix, Resource, Stat, Troop, Unit
};
enum labdscape_s : unsigned char {
    Plains, Forest, Hills, Swamp, Desert, Waste, Sea,
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
    costa       income; // Base income from landscape
};
struct nameable {
    const char* id;
};
struct stati {
    const char* id;
};
struct costi {
    const char* id;
    bool        visible;
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
struct decki : varianta {
    void        addrandom(variant v);
    void        insert(int i, variant v);
};
struct eventi {
    const char* id;
    constexpr explicit operator bool() const { return id != 0; }
    static void read(const char* url);
};
struct eventcasei {
    short       parent;
    short       id;
    int         next;
    const char* text;
    const char* image; // Possible event image. If none get from landscape.
    variants    effect;
    constexpr explicit operator bool() const { return parent != -1; }
    void        clear();
    bool        isprompt() const { return next == -1; }
};
struct playeri {
    const char* id;
    costa       total;
    decki       events;
    int         get(variant v) const;
    void        initialize();
};
struct prefixi {
    const char* id;
};
struct gamei {
    playeri*    player;
    provincei*  province;
    bool        apply(const variants& source, bool test_required = false);
    static int  get(variant v, const variants& source);
    static variant getaction(variant v);
    static int  getbonus(variant v);
    static void format(stringbuilder& sb, const char* string, ...);
    void        play(const eventi* event);
};
extern gamei    game;
namespace draw {
long            dialog(answers& an, const char* title, const char* format);
void            initialize();
}
int             getfix(stringbuilder* sb, int v, variant id);
VKIND(bonusi, Bonus)
VKIND(cost_s, Cost)
VKIND(eventi, Event)
VKIND(stat_s, Stat)
VKIND(troop, Troop)
VKIND(uniti, Unit)