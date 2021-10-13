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
    NoVariant, Resource, Unit
};
struct statable : dataset<16> {
};
struct costa : dataset<8> {
};
struct nameable {
    const char*         id;
};
struct stati {
    const char*         id;
};
struct costi {
    const char*         id;
};
struct resourcei {
    const char*         id;
};
struct resourcea : flagable<2> {
};
struct uniti : nameable {
    statable            stats;
    costa               cost, upkeep;
};
struct army : adat<variant, 18> {
    void                damage(int hits);
};
struct hero : uniti {
    army                troops;
};
struct provincei : nameable {
    costa               income;
    char                explored;
    resourcea           resources;
    army                troops;
};