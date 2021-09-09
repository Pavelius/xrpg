#pragma once

typedef void(*fnevent)();

struct menui {
	const char*			id;
	const char*			parent;
	const char*			resid;
	bool				menuback;
	static void			choose(const char* parent, const char* resid, const char* title);
};
