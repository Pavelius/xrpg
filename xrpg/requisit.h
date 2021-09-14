#include "variant.h"

#pragma once

struct valuei {
	variant				object;
	variant				requisit;
	int					value;
	constexpr explicit operator bool() const { return object.operator bool(); }
	static void			add(variant object, variant requisit, int value);
	static valuei*		find(variant object, variant requisit);
};
struct requisiti {
	const char*			id;
	variant				parent;
	variant				type;
	int					index;
};