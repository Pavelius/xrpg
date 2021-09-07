#include "flagable.h"
#include "variant.h"

#pragma once

enum variant_s : unsigned char {
	NoVariant,
	Abilities, Class, Creatures, Feats, Modifiers, Parameters, Progress, Races, Skills, Tables,
};
enum modifier_s : unsigned char {
	Need, Need13, Minus,
};
typedef flagable<4>		skilla;
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
};
struct creaturei {
	const char*			id;
	variant				race;
	statablei			current, basic;
	void				clear();
};
struct feati {
	const char*			id;
};
struct racei {
	const char*			id;
	variant				base;
	variants			feats;
};
struct skilli {
	const char*			id;
};