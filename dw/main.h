#include "crt.h"
#include "flagable.h"

#pragma once

enum itemf_s : unsigned char {
	Light, Heavy, Messy, Crushing,
	Clumsy, Armor1, Armor2, Pierce1, Pierce2,
	TwoHanded, Thrown,
};

typedef flagable<2> itemfa;

union item {
	unsigned u;
	short unsigned w[2];
	struct {
		unsigned char type;
		//
		unsigned char signature : 1;
		unsigned char used : 3;
		//
		itemfa feats;
	};
};