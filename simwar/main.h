#include "dataset.h"
#include "variant.h"

enum stat_s : unsigned char {
    Attack, Defend, Raid, Move, Damage, Hits,
    Level
};
enum consumable_s : unsigned char {
    Gold, Mana, Fame
};
enum variant_s : unsigned char {
    NoVariant, Unit
};
struct statable : dataset<stat_s, 16> {
};
struct resourceable : dataset<consumable_s, 8> {
};
struct nameable {
    const char*         id;
};
struct stati {
    const char*         id;
};
struct consumablei {
    const char*         id;
};
struct uniti : nameable {
    statable            stats;
    resourceable        cost, upkeep;
};