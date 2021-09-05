#include "flagable.h"
#include "variant.h"

#pragma once

enum variant_s : unsigned char {
	NoVariant,
	Abilities, Creatures, Feats, Races
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
struct dicei {
	const char*			id;
	char				min;
	char				max;
};
struct statablei {
	char				abilities[64];
	flagable<16>		feats;
	void				add(variant v);
	int					get(variant v) const;
};
struct creaturei : statablei {
	const char*			id;
	variant				race;
	statablei			basic;
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