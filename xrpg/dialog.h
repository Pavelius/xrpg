#include "crt.h"
#include "flagable.h"
#include "variant.h"

#pragma once

struct dialogi {
	const char*		id;
	flagable<8>		visited;
	bool			isvisited(int v) const { return visited.is(v); }
	void			setvisited(int v) { visited.set(v); }
};
struct speechi {
	int				parent;
	int				index, next_index;
	const char*		format;
	sliceu<variant>	commands;
	bool			isanswer() const { return ((unsigned)next_index) != 0xFFFFFFFF; }
	bool			isnext() const { return next_index != 0 && ((unsigned)next_index) < 0xFFFFFF00; }
};
bool				initialize_speech(const char* locale);