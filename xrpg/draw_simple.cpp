#include "answers.h"
#include "draw.h"
#include "draw_button.h"
#include "draw_input.h"
#include "draw_simple.h"
#include "handler.h"

using namespace draw;

scenei					draw::scene;
point					draw::camera;
int                     draw::pausetime;
static point			camera_drag;
static rect				board;
static const void*		current_hilite;

namespace metrics {
int                     padding = 4;
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

void draw::setpositionru() {
	width = 320;
	caret.x = getwidth() - width - metrics::border - metrics::padding;
	caret.y = metrics::border + metrics::padding;
}

void draw::setpositionlu() {
	setposition(metrics::padding * 2, metrics::padding * 2);
	width = 400;
}

void draw::setposition(int x, int y) {
	caret.x = x;
	caret.y = y;
}

void draw::setpositionld() {
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
	caret.y += textf(caret.x, caret.y, width, string);
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
		if(image_surface)
			image_height = image_surface->get(0).sy;
	}
	auto padding_height = 0;
	if(image_height && text_height)
		padding_height = metrics::padding;
	rect rc = {caret.x, caret.y, caret.x + width, caret.y + image_height + text_height + padding_height};
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
	rect rc = {caret.x, caret.y, caret.x + width, caret.y};
	textw(rc, title);
	rc.x2 = rc.x1 + width;
	rc.y2 = rc.y2 + metrics::padding;
	auto result = swindow(rc, true, 0);
	text(rc, title, AlignCenterCenter);
	caret.y += metrics::border * 2 + rc.height();
	return result;
}

bool draw::buttonrd(const char* title) {
	if(!title)
		return false;
	rect rc = {caret.x, caret.y, caret.x + width, caret.y};
	textw(rc, title);
	rc.y2 = rc.y2 + metrics::padding;
	auto result = swindow(rc, true, 0);
	text(rc, title, AlignCenterCenter);
	caret.x += rc.width() + metrics::padding * 3;
	return result;
}

bool draw::button(const char* title, unsigned key, bool(*p)(const char*), const char* description, bool* is_hilited) {
	auto hilite = p(title);
	if(hilite) {
		if(description)
			tooltips(description);
	}
	if(is_hilited)
		*is_hilited = hilite;
	return (key && hot.key == key)
		|| (hot.key == MouseLeft && hilite && !hot.pressed);
}

extern stringbuilder tooltips_sb;

void draw::answerbt(int i, long id, const char* title) {
	static char answer_hotkeys[] = {'1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I'};
	if(i >= sizeof(answer_hotkeys) / sizeof(answer_hotkeys[0]))
		i = sizeof(answer_hotkeys) / sizeof(answer_hotkeys[0]) - 1;
	bool is_hilited = false;
	if(button(title, answer_hotkeys[i], buttonfd, 0, &is_hilited))
		execute(breakparam, id);
	if(is_hilited) {
		variant value = (void*)id;
		if(value) {
			tooltips_sb.clear();
			value.getinfo(tooltips_sb);
		}
	}
}

void draw::customwindow() {
	setpositionlu();
	auto height = 300;
	swindow({caret.x, caret.y, caret.x + width, caret.y + height}, false, 0);
	tooltips(caret.x + metrics::padding, caret.y + height + metrics::padding * 4, width);
}

void draw::paintclear() {
	scene.hilite.clear();
	rectf({0, 0, getwidth(), getheight()}, colors::window);
	tooltips(metrics::padding * 3, metrics::padding * 3, 320);
}

void draw::paintimage() {
	if(!scene.resurl)
		return;
	auto p = gres(scene.resurl, "art/background");
	if(!p)
		return;
	auto& fr = p->get(0);
	board.x1 = -camera.x;
	board.y1 = -camera.y;
	if(fr.sx < getwidth())
		board.x1 = (getwidth() - fr.sx) / 2;
	if(fr.sy < getheight())
		board.y1 = (getheight() - fr.sy) / 2;
	board.x2 = board.x1 + fr.sx;
	board.y2 = board.y1 + fr.sy;
	ishilite(board);
	image(board.x1, board.y1, p, 0, 0);
}

void draw::paintcommands() {
	if(!scene.commands)
		return;
	setpositionld();
	windows(scene.commands);
}

void draw::paintall() {
	paintclear();
	paintimage();
	paintcommands();
}

void draw::set(int x, int y) {
	caret.x = x - camera.x + getwidth() / 2;
	caret.y = y - camera.y + getheight() / 2;
}

static void inputimage() {
	const int step = 32;
	switch(hot.key) {
	case KeyLeft: execute(cbsetint, camera.x - step, 0, &camera.x); break;
	case KeyRight: execute(cbsetint, camera.x + step, 0, &camera.x); break;
	case KeyUp: execute(cbsetint, camera.y - step, 0, &camera.y); break;
	case KeyDown: execute(cbsetint, camera.y + step, 0, &camera.y); break;
	case MouseWheelUp: execute(cbsetint, camera.y - step, 0, &camera.y); break;
	case MouseWheelDown: execute(cbsetint, camera.y + step, 0, &camera.y); break;
	case MouseLeft:
		if(hot.pressed && hot.hilite == board) {
			dragbegin(&camera);
			camera_drag = camera;
		}
		break;
	default:
		if(dragactive(&camera)) {
			hot.cursor = cursor::All;
			if(hot.mouse.x >= 0 && hot.mouse.y >= 0)
				camera = camera_drag + (dragmouse - hot.mouse);
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

static unsigned long getmillisecond(unsigned long start_time) {
    return (clock() - start_time) / 250;
}

void draw::status(const char* format, ...) {
    if(!pausetime)
        return;
    auto start_time = clock();
	while(ismodal() && getmillisecond(start_time) < pausetime) {
		if(scene.background)
			scene.background();
        tooltips_sb.addv(format, xva_start(format));
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
	auto push_caret = caret;
	auto push_width = width;
	setpositionru();
	auto columns = getcolumns(*this);
	auto column_width = 320;
	width = push_width;
	caret = push_caret;
	if(columns > 1)
		column_width = column_width / columns - metrics::padding;
	while(ismodal()) {
		if(scene.background)
			scene.background();
		if(scene.window)
			scene.window();
		auto push_caret = caret;
		auto push_width = width;
		setpositionru();
		window(false, title, resid);
		auto index = 0;
		auto y1 = caret.y, x1 = caret.x;
		auto y2 = caret.y;
		auto next_column = (elements.getcount() + columns - 1) / columns;
		width = column_width;
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
		width = push_width;
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
	image(caret.x, caret.y, p, 0, 0, alpha);
	width = p->get(0).sx;
	rectb({caret.x, caret.y, caret.x + width, caret.y + width});
	caret.y += p->get(0).sy + 1;
}

void draw::bar(int value, int maximum) {
	if(!value || !maximum)
		return;
	rect rc = {caret.x, caret.y, caret.x + width, caret.y + 5};
	rect r1 = rc; r1.x1++; r1.y1++;
	if(value != maximum)
		r1.x2 = r1.x1 + (rc.width() - 2) * value / maximum;
	rectf(rc, colors::form);
	rectf(r1);
	rectb(rc, colors::border);
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
		scene.background = paintall;
	tooltips_custom = custom_window;
	tooltips_use_idle = false;
}
