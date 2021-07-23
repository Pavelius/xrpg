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