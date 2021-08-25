#include "point.h"

#pragma once

typedef void(*fnevent)();

namespace draw {
struct scenei {
	fnevent			background;
	fnevent			window;
};
extern scenei		scene;
void				simplerun();
void				setpostion(point pos);
void				stext(const char* string);
bool				window(bool hilite, const char* string, const char* resid);
}
