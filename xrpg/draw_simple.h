#include "point.h"

#pragma once

typedef void(*fnevent)();

namespace draw {
struct scenei : public point {
	int				width = 320;
	const char*		resurl = 0;
	fnevent			background = 0;
	fnevent			window = 0;
};
extern scenei		scene;
void				simpleui();
void				setposition();
void				setposition(int x, int y);
void				stext(const char* string);
bool				window(bool hilite, const char* string, const char* resid);
}
