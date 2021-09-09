#include "answers.h"
#include "flagable.h"
#include "gender.h"
#include "menu.h"
#include "variant.h"

#pragma once

enum variant_s : unsigned char {
	NoVariant,
	Abilities, Class, Creatures, Feats, Menu, Modifiers, Packages, Parameters, Progress, Races, Records, Skills, Tables,
};
enum modifier_s : unsigned char {
	NoModifier,
	AbilityBonus, Need, Need13, Minus,
};
//enum dice_s : unsigned char {
//	Roll1d3, Roll1d4, Roll1d6, Roll1d8, Roll1d10, Roll1d12, Roll1d20, Roll1d100,
//	Roll2d3, Roll2d4, Roll2d6, Roll2d8, Roll2d10,
//	Roll3d3, Roll3d4, Roll3d6, Roll3d8, Roll3d10,
//	Roll4d6, Roll5d6, Roll6d6, Roll7d6, Roll8d6,
//};
typedef flagable<4>		skilla;

struct recordi {
	const char*			id;
	variant				parent;
	int					param;
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