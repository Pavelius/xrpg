#include "crt.h"

#pragma once

enum menu_flag_s : unsigned char {
	MenuBack,
};

struct menui {
	typedef cflags<menu_flag_s>	flaga;
	struct resulti {
		const char*		next;
		fnevent			proc;
	};
	const char*			parent;
	const char*			name;
	resulti				result;
	const char*			text;
	const char*			resid;
	flaga				flags;
	static void			choose(const char* parent, const char* resid, const char* title);
};
