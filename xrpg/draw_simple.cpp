#include "answers.h"
#include "draw.h"
#include "draw_input.h"
#include "draw_simple.h"

using namespace draw;

static point			pos;
static int				width = 200;

namespace metrics {
int						border = 4;
unsigned char			opacity = 186;
unsigned char			opacity_hilighted = 210;
}

static bool window(rect rc, bool hilight, int border) {
	if(border == 0)
		border = metrics::padding;
	rc.offset(-border, -border);
	color c = colors::form;
	color b = colors::form;
	rect r1 = rc; r1.offset(1, 1);
	auto rs = ishilite(r1);
	auto op = metrics::opacity;
	if(hilight && rs) {
		op = metrics::opacity_hilighted;
		if(hot.pressed)
			op = 0xFF;
	}
	rectf(rc, c, op);
	rectb(rc, b);
	return rs;
}

void draw::stext(const char* string) {
	draw::link[0] = 0;
	pos.y += textf(pos.x, pos.y, width, string);
	if(draw::link[0])
		tooltips(draw::link);
}

bool draw::window(bool hilite, const char* string, const char* resid) {
	if(!string && !resid)
		return false;
	auto text_height = 0;
	auto image_height = 0;
	const sprite* image_surface = 0;
	if(string) {
		auto push_clipping = clipping; clipping.clear();
		text_height = draw::textf(0, 0, width, string);
		clipping = push_clipping;
	}
	if(resid) {
		image_surface = gres(resid, "art/images");
		image_height = 200;
	}
	auto padding_height = 0;
	if(image_height && text_height)
		padding_height = metrics::padding;
	rect rc = {pos.x, pos.y, pos.x + width, pos.y + image_height + text_height + padding_height};
	auto rs = window(rc, hilite, 0);
	if(image_surface) {
		image(pos.x, pos.y, image_surface, 0, 0);
		pos.y += image_height + padding_height;
	}
	if(string) {
		auto py = pos.y;
		stext(string);
		pos.y = py;
	}
	pos.y += rc.height() + metrics::border * 2;
	return rs;
}

void draw::simplerun() {
	while(ismodal()) {
		domodal();
	}
}