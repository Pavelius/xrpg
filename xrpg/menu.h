#pragma once

typedef void(*fnevent)();
typedef bool(fnmenuallow)(const void* p);

struct menui {
	const char*			id;
	const char*			parent;
	const char*			resid;
	bool				menuback;
	static void			choose(const char* parent, const char* resid, const char* title);
};
