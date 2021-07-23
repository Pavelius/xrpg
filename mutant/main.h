#include "answers.h"
#include "variant.h"
#include "view.h"

#pragma once

enum variant_s : unsigned char {
	None, Attribute, Skill,
};
enum attribute_s : unsigned char {
	Strenght, Agility, Wits, Empathy
};
enum skill_s : unsigned char {
	Endure, Force, Fight, Sneak, Move, Shoot, Scout, Comprehend, KnowTheZone, SenceEmotion, Manipulate, Heal, Intimidate,
};
template<int count>
struct datamap {
	constexpr static unsigned perbyte = 4;
	unsigned char		data[(count + perbyte - 1) / perbyte];
	int					get(short unsigned i) const { return (data[i / perbyte] >> ((i % perbyte) * 2)) & 0x03; }
	void				set(short unsigned i, int v) { data[i / perbyte] |= (v & 0x03) << ((i % perbyte) * 2); }
};