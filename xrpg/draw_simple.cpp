#include "answers.h"
#include "draw.h"
#include "draw_button.h"
#include "draw_input.h"
#include "draw_simple.h"
#include "handler.h"

using namespace draw;

scenei					draw::scene;
static pointl			camera_drag;
static rect				board;

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

static void standart_background(rect rc) {
	swindow(rc, false, 0);
}

void draw::setposition() {
	scene.x = getwidth() - scene.width - metrics::border - metrics::padding;
	scene.y = metrics::border + metrics::padding;
}

void draw::setpositionrd() {
	setposition(metrics::padding * 2, getheight() - metrics::padding * 3 - texth());
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

bool draw::button(const char* title, unsigned key, bool(*p)(const char*), const char* description) {
	auto hilite = p(title);
	if(hilite && description)
		tooltips(description);
	return (key && hot.key == key)
		|| (hot.key == MouseLeft && hilite && !hot.pressed);
}

bool draw::buttonrd(const char* title) {
	if(!title)
		return false;
	rect rc = {scene.x, scene.y, scene.x + scene.width, scene.y};
	textw(rc, title);
	rc.y2 = rc.y2 + metrics::padding;
	auto result = swindow(rc, true, 0);
	text(rc, title, AlignCenterCenter);
	scene.x += rc.width() + metrics::padding * 3;
	return result;
}

void draw::answerbt(int i, long id, const char* title) {
	static char answer_hotkeys[] = {'1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I'};
	if(i >= sizeof(answer_hotkeys) / sizeof(answer_hotkeys[0]))
		i = sizeof(answer_hotkeys) / sizeof(answer_hotkeys[0]) - 1;
	if(buttonfd(title, answer_hotkeys[i], 0))
		execute(breakparam, id);
}

void draw::paintclear() {
	rectf({0, 0, getwidth(), getheight()}, colors::window);
	tooltips(metrics::padding * 3, metrics::padding * 3, 320);
}

void draw::paintimage() {
	paintclear();
	if(!scene.resurl)
		return;
	auto p = gres(scene.resurl, "art/background");
	if(!p)
		return;
	auto& fr = p->get(0);
	board.x1 = -scene.camera.x;
	board.y1 = -scene.camera.y;
	if(fr.sx < getwidth())
		board.x1 = (getwidth() - fr.sx) / 2;
	if(fr.sy < getheight())
		board.y1 = (getheight() - fr.sy) / 2;
	board.x2 = board.x1 + fr.sx;
	board.y2 = board.y1 + fr.sy;
	ishilite(board);
	image(board.x1, board.y1, p, 0, 0);
}

void draw::set(int x, int y) {
	scene.x = x - scene.camera.x + getwidth() / 2;
	scene.y = y - scene.camera.y + getheight() / 2;
}

static void inputimage() {
	const int step = 32;
	switch(hot.key) {
	case KeyLeft: execute(cbsetint, scene.camera.x - step, 0, &scene.camera.x); break;
	case KeyRight: execute(cbsetint, scene.camera.x + step, 0, &scene.camera.x); break;
	case KeyUp: execute(cbsetint, scene.camera.y - step, 0, &scene.camera.y); break;
	case KeyDown: execute(cbsetint, scene.camera.y + step, 0, &scene.camera.y); break;
	case MouseWheelUp: execute(cbsetint, scene.camera.y - step, 0, &scene.camera.y); break;
	case MouseWheelDown: execute(cbsetint, scene.camera.y + step, 0, &scene.camera.y); break;
	case MouseLeft:
		if(hot.pressed && hot.hilite == board) {
			dragbegin(&scene.camera);
			camera_drag = scene.camera;
		}
		break;
	default:
		if(dragactive(&scene.camera)) {
			hot.cursor = cursor::All;
			if(hot.mouse.x >= 0 && hot.mouse.y >= 0) {
				pointl pt;
				pt.x = dragmouse.x - hot.mouse.x;
				pt.y = dragmouse.y - hot.mouse.y;
				scene.camera = camera_drag + pt;
			}
		}
		break;
	}
}

static void inputall() {
	inputimage();
}

void draw::simpleui() {
	while(ismodal()) {
		if(scene.background)
			scene.background();
		if(scene.window)
			scene.window();
		inputall();
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
			if(buttonfd(cancel_text, KeyEscape, 0))
				execute(buttoncancel);
		}
		inputall();
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

static void choose_window() {
	scene.window = (fnevent)hot.object;
}

void draw::windows(const command* source) {
	for(auto p = source; *p; p++) {
		if(scene.window == p->proc)
			continue;
		if(buttonrd(getnm(p->id), p->key, getdescription(p->id)))
			execute(choose_window, 0, 0, (void*)p->proc);
	}
}

void draw::windows(const variant* source) {
}

void set_dark_theme();

extern fnevent tooltips_custom;
extern bool tooltips_use_idle;

static void custom_window() {
	rect rc = hot.hilite;
	rc = rc + metrics::padding;
	swindow(hot.hilite, false, 0);
}

HANDLER(before_initialize) {
	set_dark_theme();
	if(!scene.background)
		scene.background = paintimage;
	tooltips_custom = custom_window;
	tooltips_use_idle = false;
}