#pragma once

typedef void(*fnevent)();

namespace draw {
//struct awindowi {
//	int						x, y, width, height;
//	int						header_width;
//	unsigned				flags;
//};
//extern awindowi				awindow;
//void						breakmodal(long result);
//void						breakparam();
//void						buttoncancel();
//void						buttonok();
//void						cbsetsht();
//void						cbsetint();
//void						cbsetptr();
//void						doredraw();
//long						getresult();
//bool						isnext();
//void						initialize(const char* title);
//bool						ismodal();
//void						setneedupdate();
//void						setnext(fnevent v);
//void						start();
}
extern struct handler*	after_theme_change; // After initialization made
extern struct handler*	before_setfocus; // Before focus change
