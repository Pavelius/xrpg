#pragma once

enum menu_flag_s : unsigned char {
	MenuBack,
};

struct menui {
	const char*			parent;
	const char*			id;
	fnevent				proc;
	const char*			resid;
	unsigned			flags;
	static void			choose(const char* parent, const char* resid, const char* title);
};
