#include "color.h"
#include "markup.h"
#include "rect.h"
#include "stringbuilder.h"

#pragma once

namespace metrics {
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
bool						addbutton(rect& rc, bool focused, const char* t1, unsigned k1, const char* tt1);
int							addbutton(rect& rc, bool focused, const char* t1, unsigned k1, const char* tt1, const char* t2, unsigned k2, const char* tt2);
bool						button(const rect& rc, const char* title, const char* tips, unsigned key, color value, bool focused, bool checked, bool press, bool border);
void						buttonl(int& x, int y, const char* title, fnevent proc, unsigned key = 0, void* focus_value = 0);
void						buttonr(int& x, int y, const char* title, fnevent proc, unsigned key = 0);
void						checkbox(void* source, int size, unsigned bits, const char* label, const char* tips = 0);
const char*					contextmenu(const char** commands, const void* object, fnallowid allowid, fngetcommands getcommands);
bool						edit(const rect& rc, void* source, int size, unsigned align, bool isnumber);
void						fielcd(void* source, int size, array& database, const void* object, const fnlist& plist, bool instant);
void						fieln(const char* label, void* source, int size, int label_width, int digits);
void						field(const char* label, char* source, unsigned size, int label_width, fnchoose choosep);
void						field(const char* label, color& value, int header_width, const char* tips);
void						field(const char* label, int line_height, char* source, unsigned size, int label_width, fnchoose choosep);
void						field(const char* label, const char*& source, int label_width, fnchoose choosep);
void						field(const char* label, void* source, int size, int label_width, array& source_data, const fnlist& plist, const char* tips);
const char*					getenumname(const void* object, stringbuilder& sb);
int							getimage(const char* id);
unsigned					getkey(const char* id);
const char*					getpresent(void* source, int size, bool isnumber, const array* database, stringbuilder& sb);
long						getsource(void* source, int size);
void						logmsgv(const char* format, const char* arguments);
void						radio(void* source, int size, unsigned bits, const char* label, const char* tips = 0);
void						setposition();
void						setsource(void* source, int size, long value);
int							sheetline(rect rc, bool background);
void						splith(int x, int y, int width, int& value, int size, int minimum, int maximum, bool down_align);
void						splitv(int x, int y, int& value, int height, int size, int minimum, int maximum, bool right_align);
void						statusbar(const char* format, ...);
void						statusbar(rect& rc);
void						statuscolumn(int index, int width, const char* format, ...);
int							tabs(rect rc, bool show_close, bool right_side, void** data, int start, int count, int current, int* hilite, fntext gtext, int* result_x1, color back);
int							tabv(rect rc, bool show_close, bool right_side, void** data, int start, int count, int current, int* hilite, fntext gtext);
void						titletext(const char* label, int label_width, const char* separator = 0);
}