#include "crt.h"
#include "color.h"
#include "rect.h"
#include "stringbuilder.h"

#pragma once

namespace metrics {
extern int					padding;
extern int					edit;
namespace show {
extern bool					padding, statusbar;
extern bool					bottom, top, left, right;
}
}
namespace draw {
typedef bool (*fnallowid)(const void* object, const char* id);
typedef const char** (*fngetcommands)(const void* object, const char* id);
namespace dialog {
bool						choose(color& result, struct color* custom = 0);
bool						folder(const char* title, char* result);
bool						open(const char* title, char* path, const char* filter, int filter_index = -1, const char* ext = 0);
bool						save(const char* title, char* path, const char* filter, int filter_index = -1);
}
void						application();
void						application(const char* title);
bool						addbutton(rect& rc, bool focused, const char* t1, unsigned k1, const char* tt1);
int							addbutton(rect& rc, bool focused, const char* t1, unsigned k1, const char* tt1, const char* t2, unsigned k2, const char* tt2);
void						breakmodal(int result);
void						breakparam();
bool						button(const rect& rc, const char* title, const char* tips, unsigned key, color value, bool focused, bool checked, bool press, bool border);
void						buttoncancel();
void						buttonl(int& x, int y, const char* title, fnevent proc, unsigned key = 0, void* focus_value = 0);
void						buttonr(int& x, int y, const char* title, fnevent proc, unsigned key = 0);
void						buttonok();
void						cbsetint();
void						cbsetptr();
void						checkbox(int x, int& y, int width, void* source, int size, unsigned bits, const char* label, const char* tips = 0);
const char*					contextmenu(const char** commands, const void* object, fnallowid allowid, fngetcommands getcommands);
bool						edit(const rect& rc, void* source, int size, unsigned align, bool isnumber);
void						fieln(int x, int& y, int width, const char* label, void* source, int size, int label_width, int digits);
void						field(int x, int& y, int width, const char* label, char* source, unsigned size, int label_width, fnchoose choosep);
int							field(int x, int& y, int width, const char* label, color& value, int header_width, const char* tips);
void						field(int x, int& y, int width, int line_height, const char* label, char* source, unsigned size, int label_width, fnchoose choosep);
void						field(int x, int& y, int width, const char* label, const char*& source, int label_width, fnchoose choosep);
int							getimage(const char* id);
unsigned					getkey(const char* id);
const char*					getpresent(void* source, int size, bool isnumber, const array* database, stringbuilder& sb);
int							getresult();
long						getsource(void* source, int size);
void						initialize(const char* label, int width, int height, bool resize, int timer);
bool						ismodal();
void						logmsgv(const char* format, const char* arguments);
void						radio(int x, int& y, int width, void* source, int size, unsigned bits, const char* label, const char* tips = 0);
void						setnext(fnevent v);
void						setposition(int& x, int& y, int& width, int padding = -1);
void						setsource(void* source, int size, long value);
int							sheetline(rect rc, bool background);
void						splith(int x, int y, int width, int& value, int size, int minimum, int maximum, bool down_align);
void						splitv(int x, int y, int& value, int height, int size, int minimum, int maximum, bool right_align);
void						start();
void						statusbar(const char* format, ...);
void						statusbar(rect& rc);
int							tabs(rect rc, bool show_close, bool right_side, void** data, int start, int count, int current, int* hilite, fntext gtext, int* result_x1 = 0);
int							tabv(rect rc, bool show_close, bool right_side, void** data, int start, int count, int current, int* hilite, fntext gtext);
void						titletext(int& x, int y, int& width, const char* label, int label_width, const char* separator = 0);
void						tooltips(const char* format, ...);
}
extern struct handler*	after_initialize; // After initialization made
extern struct handler*	before_modal; // Before next step of modal cicle
extern struct handler*	before_input; // Before next step of modal cicle
extern struct handler*	after_input; // After input handled
extern struct handler*	before_setfocus; // Before focus change
extern struct handler*	leave_modal; // When leaving current modal loop