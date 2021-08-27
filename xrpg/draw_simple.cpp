#include "answers.h"
#include "draw.h"
#include "draw_button.h"
#include "draw_input.h"
#include "draw_simple.h"
#include "handler.h"

using namespace draw;

scenei					draw::scene;

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

void draw::setposition(int x, int y) {
	scene.x = x;
	scene.y = y;
}

void draw::setposition() {
	scene.x = getwidth() - scene.width - metrics::border - metrics::padding;
	scene.y = metrics::border + metrics::padding;
}

void draw::stext(const char* string) {
	draw::link[0] = 0;
	scene.y += textf(scene.x, scene.y, scene.width, string);
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
		text_height = draw::textf(0, 0, scene.width, string);
		clipping = push_clipping;
	}
	if(resid) {
		image_surface = gres(resid, "art/images");
		image_height = 200;
		if(image_surface)
			image_height = image_surface->get(0).sy;
	}
	auto padding_height = 0;
	if(image_height && text_height)
		padding_height = metrics::padding;
	rect rc = {scene.x, scene.y, scene.x + scene.width, scene.y + image_height + text_height + padding_height};
	auto rs = window(rc, hilite, 0);
	if(image_surface) {
		image(scene.x, scene.y, image_surface, 0, 0);
		scene.y += image_height + padding_height;
	}
	if(string)
		stext(string);
	scene.y += metrics::border * 2;
	return rs;
}

static void renderbitmap() {
	if(!scene.resurl)
		return;
	auto p = gres(scene.resurl, "art/background");
	if(!p)
		return;
	image(0, 0, p, 0, 0);
}

void draw::simpleui() {
	while(ismodal()) {
		renderbitmap();
		if(scene.background)
			scene.background();
		if(scene.window)
			scene.window();
		domodal();
	}
}

void set_dark_theme();

HANDLER(before_initialize) {
	set_dark_theme();
}