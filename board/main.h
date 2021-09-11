#include "answers.h"
#include "flagable.h"
#include "gender.h"
#include "menu.h"
#include "requisit.h"
#include "variant.h"

#pragma once

enum variant_s : unsigned char {
	NoVariant,
	Abilities, Class, Creatures, Dice, Feats, Menu, Modifiers,
	Packages, Parameters, Progress, Races, Records, Skills, Tables,
};
enum modifier_s : unsigned char {
	NoModifier,
	AbilityBonus, Need, Need13, Minus,
};
typedef flagable<4>		skilla;

struct recordi {
	const char*			id;
	variant				parent;
	int					param;
};
struct action_typei {
	const char*			id;
};
struct actioni {
	const char*			id;
	variant				type;
};
struct modifieri {
	const char*			id;
	int					value;
};
struct durationi {
	const char*			id;
};
struct sloti {
	const char*			id;
};
struct abilityi {
	const char*			id;
};
struct parameteri {
	const char*			id;
	int					base;
	variants			content;
};
struct packagei {
	const char*			id;
	variants			content;
};
struct progressi {
	variant				id;
	char				level;
};
struct dicei {
	const char*			id;
	char				min;
	char				max;
};
struct classi {
	const char*			id;
	char				hitdice;
	variant				spellprogress;
	skilla				skills;
	variants			feats;
};
struct statablei {
	char				abilities[64];
	char				actionused[8];
	flagable<16>		feats;
	skilla				skills;
	void				add(variant v);
	int					get(variant v) const;
	int					geteffect(variants source) const;
};
struct creaturei {
	const char*			id;
	variant				race;
	statablei			current, basic;
	void				clear();
};
struct feati {
	const char*			id;
	variants			effect;
};
struct racei {
	const char*			id;
	variant				base;
	variants			feats;
};
struct skilli {
	const char*			id;
};