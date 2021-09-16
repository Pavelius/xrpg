#pragma once

typedef void(*fnevent)();
typedef bool(*fnmenuallow)(const void* p);
typedef void(*fnmenuevent)(const char* parent, const char* type);

struct menui {
	const char*			id;
	const char*			parent;
	const char*			resid;
	const char*			type;
	static void			choose(const char* parent, const char* resid, const char* title);
};

extern bool				menu_break;
extern const char*		menu_resid;
extern fnmenuallow		menu_allow;
extern fnmenuevent		menu_apply;
extern fnmenuevent		menu_prepare;