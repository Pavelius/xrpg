#pragma once

typedef void(*fnevent)();

namespace draw {
struct awindowi {
	int						x, y, width, height;
	int						header_width;
	unsigned				flags;
};
extern awindowi				awindow;
void						breakmodal(long result);
void						breakparam();
void						buttoncancel();
void						buttonok();
void						cbsetsht();
void						cbsetint();
void						cbsetptr();
void						doredraw();
long						getresult();
bool						isnext();
void						initialize(const char* title);
bool						ismodal();
void						setneedupdate();
void						setnext(fnevent v);
void						start();
}
extern struct handler*	after_initialize; // After initialization made
extern struct handler*	after_input; // After input handled
extern struct handler*	after_theme_change; // After initialization made
extern struct handler*	before_initialize; // Before (and in time when) initialization made
extern struct handler*	before_modal; // Before next step of modal cicle
extern struct handler*	before_input; // Before next step of modal cicle
extern struct handler*	before_setfocus; // Before focus change
extern struct handler*	leave_modal; // When leaving current modal loop
