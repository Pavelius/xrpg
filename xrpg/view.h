#include "crt.h"
#include "rect.h"

#pragma once

struct guii {
	int						border;
	int						hero_size;
	int						padding;
	int						opacity;
	int						opacity_hilighted;
	int						tips_tab;
	int						tips_width;
	int						window_width;
	int						grid;
	void					initialize();
};
extern guii					gui;

namespace draw {
void						application();
void						fog(int x, int y, int n);
fnevent						getbackground();
void						grid();
void						initialize();
void						setbackground(fnevent proc);
void						setbitmap(const char* id);
void						setnext(fnevent v);
}
