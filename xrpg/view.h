#include "draw_control.h"
#include "draw_focus.h"
#include "point.h"
#include "rect.h"
#include "variant.h"

#pragma once

enum color_s : unsigned char {
	ColorRed, ColorGreen, ColorBlue, ColorYellow, ColorOrange,
	ColorDarkGray, ColorLightBlue, ColorLightGreen,
	ColorBlack, ColorWhite, ColorGray,
	NoColor,
};
enum figure_s : unsigned char {
	FigureCircle, FigureCircleFill, FigureClose, FigureCross,
	FigureRect, FigureTrianlge, FigureTrianlgeUp,
};
namespace draw {
struct formi {
	const char*				bitmap;
	fnevent					background;
	fnevent					window;
	const char**			commands;
	void					after() const;
	void					before() const;
};
extern formi				form;
extern point				hilite_grid;
extern variant				hilite_object;
void						avatar(int x, int y, const char* id, color_s color, rect* rc_result = 0, unsigned char alpha = 0xFF);
void						bar(rect rc, color_s color, color_s border, color_s back, int value, int maximum);
void						dialogul(int& x, int& y, int& width);
int							getresult();
void						header(int x, int& y, int width, const char* title, ...);
void						paint(int x, int y, figure_s type, int size);
void						paint(int x, int y, figure_s type, color_s color, int size);
void						paint(int x, int y, const char* name, figure_s type, int size);
void						paint(int x, int y, const char* name, figure_s type, color_s color, int size);
int							tabs(rect rc, bool show_close, bool right_side, void** data, int start, int count, int current, int* hilite, fntext gtext, int* result_x1 = 0);
int							tabv(rect rc, bool show_close, bool right_side, void** data, int start, int count, int current, int* hilite, fntext gtext);
void						fog(int x, int y, int n);
point						gethiliteback();
void						grid();
bool						ishilite(int x, int y, int r, variant v);
int							scene(fnevent input);
int							sheetline(rect rc, bool background);
}
struct guii {
	int						border;
	int						hero_size;
	int						padding;
	int						opacity;
	int						opacity_hilighted;
	int						tips_tab;
	int						window_width, left_window_width;
	int						grid;
};
extern guii					gui;