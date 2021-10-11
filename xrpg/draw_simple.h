#include "command.h"
#include "point.h"
#include "variant.h"

#pragma once

namespace draw {
extern point		camera;
extern int			grid_size;
extern variant		hilite_object;
extern const char*	image_url;
extern command*	    input_commands;
extern const char*  input_status;
extern int          pausetime;
void				answerbt(int index, long id, const char* title);
void				avatar(const char* id);
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
