#include "crt.h"
#include "flagable.h"
#include "variant.h"

#pragma once

enum variant_s : unsigned char {
	NoVariant,
	Damage, Squad,
};
enum damage_s : unsigned char {
	Cold, Explosion, Fire, Gas, Laser, Radiation, Shoots,
};
enum condition_s : unsigned char {
	GoodState, MiddleState, BadState
};
enum result_s : unsigned char {
	Fail, Partial, Success, CriticalSuccess,
};
enum effect_s : unsigned char {
	Harm, Supply, Time,
};
typedef flagable<2> damagef;
struct actioni {
	const char*	id;
	effect_s	type;
	char		value;
};
struct damagei {
	const char*	id;
};
struct squadi {
	const char*	id;
	unsigned char combat, infiltrate, research, build, repair, heal, count, cost;
	damagef	inflict, vulnerable, resist, immune;
};
struct squad {
	unsigned char type;
	unsigned char count : 4;
	unsigned char wounded : 4;
	explicit operator bool() const { return count != 0; }
	const squadi& geti() const { return bsdata<squadi>::elements[type]; }
	void create(unsigned char type);
	void getinfo(stringbuilder& sb) const;
	bool is(condition_s v) const;
	void kill();
	void wound();
};
struct missioni {
	squad allies[8];
	char danger, luck, supply;
	char throphy;
	void add(const char* id);
	void add(const squad& v);
	void clear();
	bool dangerous();
	void paint() const;
	bool lucky();
};
namespace draw {
void initialize();
void print(const char* format);
void runmission();
}
extern int roll_dices;
extern int roll_total;
extern result_s roll_result;
extern missioni mission;
bool groll(int bonus);