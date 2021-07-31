#include "draw_control.h"
#include "draw_focus.h"

using namespace draw::controls;

void scrollable::paint(const rect& rcc) {
	rect rc = rcc;
	//draw::scroll scrollv(origin.y, rc.height(), maximum.y, rc, false, wheels.y);
	//draw::scroll scrollh(origin.x, rc.width(), maximum.x, rc, true, wheels.x);
	//scrollv.correct(); scrollh.correct();
	//scrollv.input(); scrollh.input();
	control::paint(rcc);
	auto push_clip = clipping;
	setclip(rc);
	redraw(rc);
	clipping = push_clip;
	//scrollv.view(isfocused());
	//scrollh.view(isfocused());
}

rect draw::controls::scrollable::centerview(const rect& rc) {
	rect rs = rc;
	if(rc.width() > maximum.x) {
		rs.x1 = rc.x1 + (rc.width() - maximum.x) / 2;
		rs.x2 = rs.x1 + maximum.x;
	}
	if(rc.height() > maximum.y) {
		rs.y1 = rc.y1 + (rc.height() - maximum.y) / 2;
		rs.y2 = rs.y1 + maximum.y;
	}
	return rs;
}