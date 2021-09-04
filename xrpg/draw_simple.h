#include "command.h"
#include "point.h"
#include "variant.h"

#pragma once

typedef void(*fnevent)();

namespace draw {
struct scenei {
	int				grid = 32;
	const char*		resurl = 0;
	fnevent			background = 0;
	fnevent			window = 0;
};
extern point		camera;
extern point		caret;
extern scenei		scene;
extern int			width;
void				answerbt(int index, long id, const char* title);
void				avatar(const char* id, unsigned char alpha = 0xFF);
void				bar(int value, int maximum);
bool				button(const char* title, unsigned key, bool(*p)(const char*), const char* description);
bool				buttonfd(const char* title);
inline bool			buttonfd(const char* title, unsigned key, const char* description) { return button(title, key, buttonfd, description); }
bool				buttonrd(const char* title);
inline bool			buttonrd(const char* title, unsigned key, const char* description) { return button(title, key, buttonrd, description); }
void				customwindow();
bool				ishilite(int s, const void* object);
void				fog(int n);
void				grid();
void				paintclear();
void				paintimage();
void				simpleui();
void				set(int x, int y);
void				setposition(int x, int y);
void				setpositionru();
void				setpositionlu();
void				setpositionld();
void				sheader(const char* string);
void				stext(const char* string);
bool				window(bool hilite, const char* string, const char* resid);
void				windows(const command* source);
}
