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

static bool swindow(rect rc, bool hilight, int border) {
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
		if(image_surface)
			image_height = image_surface->get(0).sy;
	}
	auto padding_height = 0;
	if(image_height && text_height)
		padding_height = metrics::padding;
	rect rc = {scene.x, scene.y, scene.x + scene.width, scene.y + image_height + text_height + padding_height};
	auto rs = swindow(rc, hilite, 0);
	if(image_surface) {
		image(scene.x, scene.y, image_surface, 0, 0);
		scene.y += image_height + padding_height;
	}
	if(string)
		stext(string);
	scene.y += metrics::border * 2;
	return rs;
}

bool draw::buttonfd(const char* title) {
	if(!title)
		return false;
	rect rc = {scene.x, scene.y, scene.x + scene.width, scene.y};
	textw(rc, title);
	rc.x2 = rc.x1 + scene.width;
	rc.y2 = rc.y2 + metrics::padding;
	auto result = swindow(rc, true, 0);
	text(rc, title, AlignCenterCenter);
	scene.y += metrics::border * 2 + rc.height();
	return result;
}

bool draw::buttonfd(const char* title, unsigned key) {
	auto hilite = buttonfd(title);
	return (key && hot.key == key)
		|| (hot.key == MouseLeft && hilite && !hot.pressed);
}

void draw::answerbt(int i, long id, const char* title) {
	static char answer_hotkeys[] = {'1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I'};
	if(i >= sizeof(answer_hotkeys) / sizeof(answer_hotkeys[0]))
		i = sizeof(answer_hotkeys) / sizeof(answer_hotkeys[0]) - 1;
	if(buttonfd(title, answer_hotkeys[i]))
		execute(breakparam, id);
}

void draw::paintclear() {
	rectf({0, 0, getwidth(), getheight()}, colors::window);
}

void draw::paintimage() {
	if(!scene.resurl)
		return;
	auto p = gres(scene.resurl, "art/background");
	if(!p)
		return;
	auto& fr = p->get(0);
	auto x = 0, y = 0;
	if(fr.sx < getwidth())
		x = (getwidth() - fr.sx) / 2;
	if(fr.sy < getheight())
		y = (getheight() - fr.sy) / 2;
	if(x || y)
		paintclear();
	image(x, y, p, 0, 0);
}

void draw::simpleui() {
	while(ismodal()) {
		if(scene.background)
			scene.background();
		if(scene.window)
			scene.window();
		domodal();
	}
}

long answers::choose(const char* title, const char* cancel_text, bool interactive, const char* resid) const {
	if(!interactive)
		return random();
	while(ismodal()) {
		if(scene.background)
			scene.background();
		if(scene.window)
			scene.window();
		setposition();
		auto y1 = scene.y, x1 = scene.y;
		window(false, title, resid);
		auto index = 0;
		for(auto& e : *this)
			answerbt(index++, e.id, e.text);
		if(cancel_text) {
			if(buttonfd(cancel_text, KeyEscape))
				execute(buttoncancel);
		}
		domodal();
	}
	return getresult();
}

void draw::grid() {
	auto push = fore;
	fore = colors::border;
	auto x2 = getwidth(), y2 = getheight();
	auto sx = scene.grid, sy = scene.grid;
	for(auto x = 0; x < x2; x += sx)
		rectf({x, 0, x + 1, y2}, colors::border, 64);
	for(auto y = 0; y < y2; y += sy)
		rectf({0, y, x2, y + 1}, colors::border, 64);
	fore = push;
}

void draw::fog(int n) {
	if(n == 0xFF)
		rectf({scene.x, scene.y, scene.x + scene.grid, scene.y + scene.grid}, colors::black);
	else
		rectf({scene.x, scene.y, scene.x + scene.grid, scene.y + scene.grid}, colors::black, n);
}

void set_dark_theme();

HANDLER(before_initialize) {
	set_dark_theme();
	if(!scene.background)
		scene.background = paintimage;
}