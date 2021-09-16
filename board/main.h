#include "answers.h"
#include "flagable.h"
#include "gender.h"
#include "menu.h"
#include "requisit.h"
#include "variant.h"

#pragma once

enum variant_s : unsigned char {
	NoVariant,
	Abilities, Class, Conditions, Creatures, Dice, Feats, Genders, Menu, Modifiers,
	Packages, Parameters, Progress, Races, Records, Skills, Tables, Types,
};
enum modifier_s : unsigned char {
	AbilityBonus, Need, Need13, Minus,
};
enum condition_s : unsigned char {
	MatchParent, IgnoreFirst,
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
	modifier_s			base;
};
struct conditioni {
	const char*			id;
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
	bool				flags;
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
struct nameable {
	const char*			name;
	gender_s			gender;
};
struct statablei {
	char				abilities[16];
	char				parameters[64];
	char				actionused[8];
	flagable<16>		feats;
	skilla				skills;
	void				add(variant v) { set(v, get(v) + 1); }
	int					get(variant v) const;
	void				set(variant i, int v);
	int					geteffect(variants source) const;
};
struct creaturei : public nameable {
	const char*			id;
	unsigned char		race;
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
VKIND(condition_s, Conditions)
VKIND(modifier_s, Modifiers)
VKIND(variant_s, Types)