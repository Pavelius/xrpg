#include "command.h"
#include "variant.h"

#pragma once

namespace draw {
extern int			grid_size;
extern const void*	hilite_object;
extern command*	    input_commands;
extern int          pausetime;
void				answerbt(int index, long id, const char* title);
void				avatar(const char* id);
void				bar(int value, int maximum);
bool				buttonfd(const char* title);
bool				buttonft(const char* title);
bool				buttonrd(const char* title);
void				customwindow();
bool				ishilite(int s, const void* object);
void				moving(point& result, point target, int velocity = 12);
void				fog(int n);
void				grid();
void				paintclear();
void				paintcommands();
void				set(int x, int y);
void				stext(const char* string);
void                status(const char* format, ...);
bool				swindow(bool hilight);
void				texth2w(const char* string);
bool				window(bool hilite, const char* string, const char* resid);
void				windows(const command* source);
namespace simpleui {
void tips();
}
}
