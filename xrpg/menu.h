#include "cflags.h"

#pragma once

enum menu_flag_s : unsigned char {
	MenuBack,
};

struct menui {
	typedef cflags<menu_flag_s>	flaga;
	const char*			parent;
	const char*			id;
	fnevent				proc;
	const char*			resid;
	flaga				flags;
	static void			choose(const char* parent, const char* resid, const char* title);
};
