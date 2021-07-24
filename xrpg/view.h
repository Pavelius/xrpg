#include "rect.h"
#include "variant.h"

#pragma once

enum color_s : unsigned char {
	ColorRed, ColorGreen, ColorBlue,
	ColorBlack, ColorWhite, ColorGray,
};
enum figure_s : unsigned char {
	FigureCircle, FigureRect, FigureTrianlge
};
typedef adat<variant, 128>	varianta;
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
void						figure(int x, int y, figure_s figure, int size);
void						fog(int x, int y, int n);
fnevent						getbackground();
void						grid();
void						initialize();
void						scene(varianta& objects);
void						setbackground(fnevent proc);
void						setbitmap(const char* id);
void						setfore(color_s v);
void						setnext(fnevent v);
}