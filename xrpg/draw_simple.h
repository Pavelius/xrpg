#include "command.h"
#include "point.h"
#include "variant.h"

#pragma once

typedef void(*fnevent)();

namespace draw {
struct scenei {
	int				grid = 32;
	const char*		resurl = 0;
	const command*	commands = 0;
	variant			hilite;
	fnevent			background = 0;
	fnevent			window = 0;
	fnevent			doinput = 0;
	const char*     status = 0;
};
extern point		camera;
extern point		caret;
extern scenei		scene;
extern int          pausetime;
extern int			tab_pixels;
extern int			width;
void				answerbt(int index, long id, const char* title);
void				avatar(const char* id, unsigned char alpha = 0xFF);
void				bar(int value, int maximum);
bool				button(const char* title, unsigned key, bool(*p)(const char*), const char* description, bool* is_hilited);
bool				buttonfd(const char* title);
inline bool			buttonfd(const char* title, unsigned key, const char* description) { return button(title, key, buttonfd, description, 0); }
bool				buttonrd(const char* title);
inline bool			buttonrd(const char* title, unsigned key, const char* description) { return button(title, key, buttonrd, description, 0); }
void				customwindow();
void				inputall();
void				inputcamera();
bool				ishilite(int s, const void* object);
void				moving(point& result, point target, int velocity = 12);
void				fog(int n);
void				grid();
void				paintall();
void				paintclear();
void				paintimage();
void				paintcommands();
void				simpleui();
void				set(int x, int y);
void				setposition(int x, int y);
void				setpositionru();
void				setpositionlu();
void				setpositionld();
void				sheader(const char* string);
void				slide(point pt, int step = 12);
void				stext(const char* string);
void                status(const char* format, ...);
bool				window(bool hilite, const char* string, const char* resid);
void				windows(const command* source);
}
