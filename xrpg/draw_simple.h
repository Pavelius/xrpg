#include "color.h"
#include "command.h"
#include "pointl.h"
#include "variant.h"

#pragma once

typedef void(*fnevent)();

namespace draw {
enum color_s : unsigned char {
	ColorRed, ColorGreen, ColorBlue, ColorYellow, ColorOrange,
	ColorDarkGray, ColorLightBlue, ColorLightGreen,
	ColorBlack, ColorWhite, ColorGray,
	NoColor,
};
struct scenei : public pointl {
	int				width = 320, grid = 32;
	pointl			camera;
	const char*		resurl = 0;
	fnevent			background = 0;
	fnevent			window = 0;
};
extern color		fore;
extern scenei		scene;
void				answerbt(int index, long id, const char* title);
void				avatar(const char* id, unsigned char alpha = 0xFF);
void				bar(int value, int maximum);
bool				button(const char* title, unsigned key, bool(*p)(const char*), const char* description);
bool				buttonfd(const char* title);
inline bool			buttonfd(const char* title, unsigned key, const char* description) { return button(title, key, buttonfd, description); }
bool				buttonrd(const char* title);
inline bool			buttonrd(const char* title, unsigned key, const char* description) { return button(title, key, buttonrd, description); }
bool				ishilite(int s, const void* object);
void				fog(int n);
color				get(color_s v);
void				grid();
void				paintclear();
void				paintimage();
void				simpleui();
void				set(int x, int y);
void				set(color_s v);
void				setposition();
void				setpositionlu();
void				setpositionrd();
inline void			setposition(int x, int y) { scene.x = x; scene.y = y; }
void				sheader(const char* string);
void				stext(const char* string);
bool				window(bool hilite, const char* string, const char* resid);
void				windows(const command* source);
void				windows(const variant* source);
}
