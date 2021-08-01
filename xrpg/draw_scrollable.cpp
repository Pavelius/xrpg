#include "draw.h"
#include "draw_control.h"
#include "draw_focus.h"
#include "draw_scroll.h"

using namespace draw::controls;

void scrollable::paint(const rect& rcc) {
	rect rc = rcc;
	draw::scroll scrollv(origin_y, rc.height(), maximum_y, rc, false, wheel_y);
	draw::scroll scrollh(origin_x, rc.width(), maximum_x, rc, true, wheel_x);
	scrollv.correct(); scrollh.correct();
	scrollv.input(); scrollh.input();
	control::paint(rcc);
	auto push_clip = clipping;
	setclip(rc);
	redraw(rc);
	clipping = push_clip;
	scrollv.view(isfocused());
	scrollh.view(isfocused());
}

rect draw::controls::scrollable::centerview(const rect& rc) {
	rect rs = rc;
	if(rc.width() > maximum_x) {
		rs.x1 = rc.x1 + (rc.width() - maximum_x) / 2;
		rs.x2 = rs.x1 + maximum_x;
	}
	if(rc.height() > maximum_y) {
		rs.y1 = rc.y1 + (rc.height() - maximum_y) / 2;
		rs.y2 = rs.y1 + maximum_y;
	}
	return rs;
}