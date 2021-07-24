#include "draw.h"
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
struct hotkey {
	fnevent					proc;
	const char*				name;
	unsigned				key;
	constexpr operator bool() const { return proc != 0; }
};
extern point				hilite_grid;
extern variant				hilite_object;
void						avatar(int x, int y, const char* id, color_s color, rect* rc_result = 0, unsigned char alpha = 0xFF);
void						application();
void						bar(rect rc, color_s color, color_s border, color_s back, int value, int maximum);
void						breakmodal(int result);
void						breakparam();
void						buttonr(int& x, int y, const char* title, fnevent proc, unsigned key = 0);
void						buttoncancel();
void						buttonok();
void						execute(fnevent proc, int value = 0, int value2 = 0, void* object = 0);
bool						execute(const hotkey* source);
int							getresult();
void						paint(int x, int y, figure_s type, int size);
void						paint(int x, int y, figure_s type, color_s color, int size);
void						paint(int x, int y, const char* name, figure_s type, int size);
void						paint(int x, int y, const char* name, figure_s type, color_s color, int size);
void						fog(int x, int y, int n);
point						gethiliteback();
void						grid();
bool						ishilite(int x, int y, int r, variant v);
void						initialize();
int							scene(fnevent input);
void						setnext(fnevent v);
void						tooltips(int x1, int y1, int width, const char* format, ...);
bool						window(rect rc, bool hilight, int border);
bool						window(int x, int& y, int width, bool hilite, const char* string, const char* resid);
}
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
	const char*				bitmap;
	fnevent					background;
	void					initialize();
};
extern guii					gui;