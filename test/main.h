#include "crt.h"
#include "variant.h"

#pragma once

enum variant_s : unsigned char {
	NoVariant, Trait, Planet
};
struct traiti {
	const char*			id;
	int					weight;
};
struct planeti {
	const char*			name;
	int					weight;
};