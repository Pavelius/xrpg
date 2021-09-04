#include "answers.h"
#include "draw.h"
#include "draw_button.h"
#include "draw_input.h"
#include "draw_simple.h"
#include "handler.h"

using namespace draw;

scenei					draw::scene;
static point			camera_drag;
static rect				board;
static const void*		current_hilite;

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

bool draw::ishilite(int s, const void* object) {
	if(!ishilite({caret.x - s, caret.y - s, caret.x + s, caret.y + s}))
		return false;
	current_hilite = object;
	return true;
}

void draw::setposition() {
	caret.x = getwidth() - scene.width - metrics::border - metrics::padding;
	caret.y = metrics::border + metrics::padding;
}

void draw::setpositionlu() {
	setposition(metrics::padding * 2, metrics::padding * 2);
}

void draw::setposition(int x, int y) {
	caret.x = x;
	caret.y = y;
}

void draw::setpositionrd() {
	setposition(metrics::padding * 2, getheight() - metrics::padding * 3 - texth());
}

void draw::sheader(const char* string) {
	auto push_font = font;
	auto push_fore = fore;
	font = metrics::h2;
	fore = colors::h2;
	text(caret.x, caret.y, string);
	caret.y += texth() + 2;
	fore = push_fore;
	font = push_font;
}

void draw::stext(const char* string) {
	draw::link[0] = 0;
	caret.y += textf(caret.x, caret.y, scene.width, string);
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
	rect rc = {caret.x, caret.y, caret.x + scene.width, caret.y + image_height + text_height + padding_height};
	auto rs = swindow(rc, hilite, 0);
	if(image_surface) {
		image(caret.x, caret.y, image_surface, 0, 0);
		caret.y += image_height + padding_height;
	}
	if(string)
		stext(string);
	caret.y += metrics::border * 2;
	return rs;
}

bool draw::buttonfd(const char* title) {
	if(!title)
		return false;
	rect rc = {caret.x, caret.y, caret.x + scene.width, caret.y};
	textw(rc, title);
	rc.x2 = rc.x1 + scene.width;
	rc.y2 = rc.y2 + metrics::padding;
	auto result = swindow(rc, true, 0);
	text(rc, title, AlignCenterCenter);
	caret.y += metrics::border * 2 + rc.height();
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
	rect rc = {caret.x, caret.y, caret.x + scene.width, caret.y};
	textw(rc, title);
	rc.y2 = rc.y2 + metrics::padding;
	auto result = swindow(rc, true, 0);
	text(rc, title, AlignCenterCenter);
	caret.x += rc.width() + metrics::padding * 3;
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
	caret.x = x - scene.camera.x + getwidth() / 2;
	caret.y = y - scene.camera.y + getheight() / 2;
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
			if(hot.mouse.x >= 0 && hot.mouse.y >= 0)
				scene.camera = camera_drag + (dragmouse - hot.mouse);
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

static int getcolumns(const answers& an) {
	auto divider = an.getcount() % 2;
	if(an.getcount() <= 4 && divider > 0)
		return 1;
	for(auto& e : an) {
		auto len = zlen(e.text);
		if(len > 20)
			return 1;
	}
	return 2;
}

long answers::choose(const char* title, const char* cancel_text, bool interactive, const char* resid) const {
	if(!interactive)
		return random();
	auto columns = getcolumns(*this);
	auto column_width = scene.width;
	if(columns > 1)
		column_width = column_width / columns - metrics::padding;
	while(ismodal()) {
		if(scene.background)
			scene.background();
		if(scene.window)
			scene.window();
		setposition();
		window(false, title, resid);
		auto index = 0;
		auto y1 = caret.y, x1 = caret.y;
		auto y2 = caret.y;
		auto next_column = (elements.getcount() + columns - 1) / columns;
		auto push_width = scene.width;
		scene.width = column_width;
		for(auto& e : *this) {
			answerbt(index, e.id, e.text);
			if(caret.y > y2)
				y2 = caret.y;
			if((index % next_column) == next_column - 1) {
				caret.y = y1;
				caret.x += column_width + metrics::padding * 2;
			}
			index++;
		}
		caret.x = x1; caret.y = y2;
		scene.width = push_width;
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
		rectf({caret.x, caret.y, caret.x + scene.grid, caret.y + scene.grid}, colors::black);
	else
		rectf({caret.x, caret.y, caret.x + scene.grid, caret.y + scene.grid}, colors::black, n);
}

void draw::avatar(const char* id, unsigned char alpha) {
	auto p = gres(id, "art/portraits");
	if(!p)
		return;
	rect rc;
	rc.x1 = caret.x - p->width / 2;
	rc.x2 = rc.x1 + p->width;
	rc.y1 = caret.y - p->height / 2;
	rc.y2 = rc.y1 + p->height;
	image(rc.x1, rc.y1, p, 0, 0, alpha);
}

void draw::bar(int value, int maximum) {
	if(!value || !maximum)
		return;
	auto push_fore = fore;
	rect rc = {caret.x, caret.y, caret.x + scene.width, caret.y + 6};
	rect r1 = rc; r1.x1++; r1.y1++;
	if(value != maximum)
		r1.x2 = r1.x1 + (rc.width() - 2) * value / maximum;
	//	if(back != NoColor) {
	//		fore = getcolor(back);
	//		rectf(rc);
	//	}
	fore = push_fore;
	rectf(r1);
	fore = colors::border;
	rectb(rc);
	fore = push_fore;
	caret.y += rc.height();
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