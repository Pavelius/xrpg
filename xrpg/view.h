#include "rect.h"
#include "variant.h"

#pragma once

enum color_s : unsigned char {
	ColorRed, ColorGreen, ColorBlue,
	ColorBlack, ColorWhite, ColorGray,
};
enum figure_s : unsigned char {
	FigureCircle, FigureCross, FigureRect, FigureTrianlge, FigureTrianlgeUp,
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
extern point				hilite_grid;
extern variant				hilite_object;
void						application();
void						paint(int x, int y, figure_s type, int size);
void						paint(int x, int y, const char* name, figure_s type, int size);
void						fog(int x, int y, int n);
fnevent						getbackground();
void						grid();
bool						ishilited(int x, int y, int r, variant v);
void						initialize();
void						scene(fnevent proc, fnevent timer);
void						setbackground(fnevent proc);
void						setbitmap(const char* id);
void						setfore(color_s v);
void						setnext(fnevent v);
}