#include "dataset.h"
#include "flagable.h"
#include "variant.h"

enum stat_s : unsigned char {
    Attack, Defend, Raid, Move, Damage, Hits,
    Level
};
enum cost_s : unsigned char {
    Gold, Mana, Fame
};
enum variant_s : unsigned char {
    NoVariant, Bonus, Landscape, Player, Resource, Stat, Troop, Unit
};
enum labdscape_s : unsigned char {
    Plains, Forest, Hills, Swamp, Desert, Waste, Sea,
};
struct bonusi {
    const char* id;
    variant     type;
    int         bonus;
};
struct costa : dataset<8> {
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
struct resourcea : flagable<2> {
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
    char        explored; // Total exploration percent
};
struct sitetypei : nameable {
    landscapea  landscape; // Landscape types where site might generate. 0 - for all sites.
    variants    feats;
    variant     dialog;
};
struct sitei {
    variant     type;
    variant     owner;
    char        visibile;
};
struct eventi {
    const char* id;
    variants    effect;
};
struct playeri {
    const char* id;
    costa       income, total;
};
namespace draw {
void            initialize();
}
int             getfix(stringbuilder* sb, int v, variant id);
VKIND(bonusi, Bonus)
VKIND(stat_s, Stat)
VKIND(troop, Troop)
VKIND(uniti, Unit)