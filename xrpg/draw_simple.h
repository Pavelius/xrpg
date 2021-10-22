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
bool				buttonfd(const char* title);
bool				buttonrd(const char* title);
void				customwindow();
void				inputcamera();
void				inputall();
bool				ishilite(int s, const void* object);
void				moving(point& result, point target, int velocity = 12);
void				fog(int n);
void				grid();
void				paintall();
void				paintclear();
void				paintimage();
void				paintcommands();
void				set(int x, int y);
void				slide(point pt, int step = 12);
void				stext(const char* string);
void                status(const char* format, ...);
bool				swindow(bool hilight, int border = 0);
bool				window(bool hilite, const char* string, const char* resid);
void				windows(const command* source);
}
