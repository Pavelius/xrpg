#include "answers.h"
#include "draw.h"
#include "draw_button.h"
#include "handler.h"
#include "variant.h"
#include "view.h"

using namespace draw;

static point			camera;
static point			camera_drag;
static rect				last_board;
static char				text_tooltips[4096];
static stringbuilder	sb_tooltips(text_tooltips);
guii					gui; template<> array bsdata<guii>::source(&gui, sizeof(gui), 1, 1);
static sprite*			small_font = (sprite*)loadb("art/fonts/small.pma");
static char				answer_hotkeys[] = {'1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};
point					draw::hilite_grid;
variant					draw::hilite_object;
formi					draw::form;
static void*			current_focus;
static int				dialog_y;

long					distance(point p1, point p2);

static color getcolor(color_s v) {
	static color theme_colors[] = {
		{235, 90, 70}, {97, 189, 79}, {0, 121, 191}, {242, 214, 0}, {255, 159, 26},
		{52, 69, 99}, {0, 194, 224}, {81, 232, 152},
		{0, 0, 0}, {255, 255, 255}, {179, 186, 197},
	};
	return theme_colors[v];
}

static bool window(rect rc, bool hilight, int border) {
	if(border == 0)
		border = gui.border;
	rc.offset(-border, -border);
	color c = colors::form;
	color b = colors::form;
	rect r1 = rc; r1.offset(1, 1);
	auto rs = ishilite(r1);
	auto op = gui.opacity;
	if(hilight && rs) {
		op = gui.opacity_hilighted;
		if(hot.pressed)
			op = 0xFF;
	}
	rectf(rc, c, op);
	rectb(rc, b);
	return rs;
}

static int render_text(int x, int y, int width, const char* string) {
	draw::link[0] = 0;
	auto result = textf(x, y, width, string);
	if(draw::link[0])
		tooltips(draw::link);
	return result;
}

static bool window(int x, int& y, int width, bool hilite, const char* string, const char* resid) {
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
		padding_height = gui.padding;
	rect rc = {x, y, x + width, y + image_height + text_height + padding_height};
	auto rs = window(rc, hilite, 0);
	auto y0 = y;
	if(image_surface) {
		image(x, y0, image_surface, 0, 0);
		y0 += image_height + padding_height;
	}
	if(string)
		render_text(x, y0, width, string);
	y += rc.height() + gui.border * 2;
	return rs;
}

static bool windowp(int x, int& y, int width, bool hilite, const char* string, const char* resid) {
	if(!resid)
		return window(x, y, width, hilite, string, 0);
	if(!string)
		return false;
	auto ps = gres(resid);
	auto x0 = x + gui.hero_size + gui.border; width -= x0 - x;
	auto text_height = 0;
	if(string) {
		auto push_clipping = clipping; clipping.clear();
		text_height = draw::textf(0, 0, width, string);
		clipping = push_clipping;
	}
	rect rc = {x, y, x0 + width, y + imax(text_height, gui.hero_size)};
	auto rs = window(rc, hilite, 0);
	image(x, y, ps, 0, 0);
	render_text(x0, y, width, string);
	y += rc.height() + gui.border * 2;
	return rs;
}

static point getscreen(point pt) {
	auto x = pt.x - camera.x + last_board.width() / 2;
	auto y = pt.y - camera.y + last_board.height() / 2;
	return {(short)x, (short)y};
}

static point getmappos(const rect& rc, point pt) {
	auto x = pt.x + camera.x - rc.x1 - rc.width() / 2;
	auto y = pt.y + camera.y - rc.y1 - rc.height() / 2;
	return {(short)x, (short)y};
}

static bool control_board() {
	const int step = 32;
	switch(hot.key) {
	case KeyLeft: camera.x -= step; break;
	case KeyRight: camera.x += step; break;
	case KeyUp: camera.y -= step; break;
	case KeyDown: camera.y += step; break;
	case MouseWheelUp:
		camera.y -= step;
		break;
	case MouseWheelDown:
		camera.y += step;
		break;
	case MouseLeft:
		if(hot.pressed && last_board == hot.hilite) {
			dragbegin(&last_board);
			camera_drag = camera;
		} else
			return false;
		break;
	default:
		if(dragactive(&last_board)) {
			hot.cursor = cursor::All;
			if(hot.mouse.x >= 0 && hot.mouse.y >= 0)
				camera = camera_drag + (dragmouse - hot.mouse);
			return true;
		}
		return false;
	}
	return true;
}

void draw::tooltips(const char* format, ...) {
	sb_tooltips.addv(format, xva_start(format));
}

static void render_tooltips() {
	if(!text_tooltips[0])
		return;
	rect rc;
	rc.x1 = gui.border * 2;
	rc.y1 = dialog_y;
	rc.x2 = rc.x1 + gui.left_window_width;
	rc.y2 = rc.y1;
	draw::textf(rc, text_tooltips, gui.tips_tab);
	// Correct border
	int height = draw::getheight();
	int width = draw::getwidth();
	if(rc.y2 >= height)
		rc.move(0, height - 2 - rc.y2);
	if(rc.x2 >= width)
		rc.move(width - 2 - rc.x2, 0);
	window(rc, false, 0);
	draw::fore = colors::tips::text;
	draw::textf(rc.x1, rc.y1, rc.width(), text_tooltips, 0, 0, 0, 0, gui.tips_tab);
	dialog_y += rc.height() + gui.border * 2;
}

static void standart_domodal() {
	render_tooltips();
	hot.key = draw::rawinput();
	if(!hot.key)
		exit(0);
	if(inputfocus())
		return;
	if(control_board())
		return;
}

void ismodal_update() {
	sb_tooltips.clear();
	hilite_object.clear();
	hilite_grid = {-100, -100};
	dialog_y = gui.border * 2;
	domodal = standart_domodal;
	clearfocus();
}

void ismodal_leaving() {
	setfocus(0, 0, true);
}

static void setwindow(int& x, int& y) {
	x = getwidth() - gui.window_width - gui.border * 2;
	y = gui.border * 2;
}

static void shadow(const rect& rc, color c) {
	auto op = gui.opacity;
	rectf(rc, c, op);
}

static int status(int x, int y, int width, const char* format, const char* title, fnevent proc = 0, unsigned key = 0) {
	rect rc = {x - 1, y - 1, x + width + 1, y + 1 + texth()};
	auto hilite = ishilite(rc);
	if(proc) {
		auto pressed = hot.pressed;
		auto need_execute = false;
		shadow(rc, hilite ? (pressed ? colors::button.darken() : colors::button) : colors::form);
		if(key && hot.key == key)
			need_execute = true;
		if(hot.key == MouseLeft && hilite && !hot.pressed)
			need_execute = true;
		if(need_execute)
			execute(proc);
	} else
		shadow(rc, colors::form);
	text(x + (width - textw(format)) / 2, y, format);
	if(hilite)
		tooltips(title);
	return x + width + gui.border;
}

static int status(int x, int y, int width, int value, const char* title) {
	char temp[32]; stringbuilder sb(temp);
	sb.add("%1i", value);
	return status(x, y, width, temp, title);
}

static void buttonw(int& x, int y, const char* title, fnevent proc, unsigned key) {
	auto h = texth() + 8;
	auto w = textw(title) + 8;
	rect r = {x, y, x + w, y + h};
	auto focus = isfocused(r, (void*)proc);
	auto result = button(r, title, 0, key, colors::button, focus, false, false, true);
	if(result)
		execute(cbsetptr, (long)proc, 0, &form.window);
	x += w + gui.padding;
}

static void paint_commands() {
	int x = gui.border;
	int y = getheight() - texth() - 8 - gui.border;
	if(!form.commands)
		return;
	for(auto p = form.commands; *p; p++) {
		auto id = *p;
		auto fn = getcommand(id);
		if(!fn || form.window==fn)
			continue;
		auto nm = getnm(id);
		if(!nm)
			continue;
		buttonw(x, y, nm, fn, 0);
	}
}

void formi::before() const {
	if(bitmap)
		image(0, 0, gres(bitmap, "art/background"), 0, 0);
	else
		rectf({0, 0, getwidth(), getheight()}, colors::gray);
	if(background)
		background();
	if(window)
		window();
	paint_commands();
}

void formi::after() const {
	if(hilite_object)
		hilite_object.getinfo(sb_tooltips);
}

static void answer_button(int x, int& y, int width, long id, const char* string, unsigned key) {
	auto text_height = 0;
	text_height = texth(string, width);
	rect rc = {x, y, x + width, y + text_height};
	auto rs = window(rc, true, 0);
	text(rc, string, AlignCenterCenter);
	y += rc.height() + gui.border * 2;
	auto need_execute = false;
	if(rs) {
		variant v = (void*)id;
		if(v)
			hilite_object = v;
		if(hot.key == MouseLeft && !hot.pressed)
			need_execute = true;
	}
	if(key && hot.key == key)
		need_execute = true;
	if(need_execute)
		execute(breakparam, id);
}

void draw::dialogul(int& x, int& y, int& w) {
	x = gui.border * 2;
	y = dialog_y;
	w = getwidth() - gui.window_width - gui.border * 4 - gui.padding - x;
	auto h = getheight() / 2;
	window({x, y, x + w, y + h}, false, 0);
	dialog_y += h + gui.border * 2 + gui.padding;
}

void draw::header(int x, int& y, int width, const char* title, ...) {
	if(!title || !title[0])
		return;
	char temp[260]; stringbuilder sb(temp);
	sb.addv(title, xva_start(title));
	auto push_font = font;
	auto push_fore = fore;
	font = metrics::h2;
	fore = colors::h2;
	text(x, y, temp);
	y += texth() + 2;
	fore = push_fore;
	font = push_font;
}

int getcolumns(const answers& an) {
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
	int x, y;
	auto columns = getcolumns(*this);
	auto column_width = gui.window_width;
	if(columns > 1)
		column_width = column_width / columns - gui.border;
	while(ismodal()) {
		form.before();
		setwindow(x, y);
		window(x, y, gui.window_width, false, title, resid);
		y += gui.padding;
		auto index = 0;
		auto next_column = (elements.getcount() + columns - 1) / columns;
		auto x1 = x;
		auto y1 = y;
		auto y2 = y;
		for(auto& e : elements) {
			auto i = index;
			if(i>=sizeof(answer_hotkeys) / sizeof(answer_hotkeys[0]))
				i = sizeof(answer_hotkeys) / sizeof(answer_hotkeys[0]) - 1;
			answer_button(x, y, column_width, e.id, e.text, answer_hotkeys[i]);
			if(y > y2)
				y2 = y;
			if((index % next_column) == next_column - 1) {
				y = y1;
				x += column_width + gui.border * 2;
			}
			index++;
		}
		x = x1; y = y2;
		if(cancel_text) {
			y += gui.padding;
			answer_button(x, y, gui.window_width, 0, cancel_text, KeyEscape);
		}
		form.after();
		domodal();
	}
	return getresult();
}

int draw::scene(fnevent input) {
	while(ismodal()) {
		form.before();
		form.after();
		domodal();
		if(input)
			input();
	}
	return getresult();
}

void draw::grid() {
	auto push = fore;
	fore = colors::border;
	auto x2 = getwidth(), y2 = getheight();
	auto sx = gui.grid, sy = gui.grid;
	for(auto x = 0; x < x2; x += sx)
		rectf({x, 0, x + 1, y2}, colors::border, 64);
	for(auto y = 0; y < y2; y += sy)
		rectf({0, y, x2, y + 1}, colors::border, 64);
	fore = push;
}

void draw::fog(int x, int y, int n) {
	auto x1 = x * gui.grid, y1 = y * gui.grid;
	if(n == 0xFF)
		rectf({x1, y1, x1 + gui.grid, y1 + gui.grid}, colors::black);
	else
		rectf({x1, y1, x1 + gui.grid, y1 + gui.grid}, colors::black, n);
}

void draw::paint(int x, int y, figure_s type, int size) {
	switch(type) {
	case FigureCircle:
		circle(x, y, size);
		break;
	case FigureCircleFill:
		circlef(x, y, size, fore, 128);
		circle(x, y, size);
		break;
	case FigureRect:
		rectb({x - size, y - size, x + size, y + size});
		break;
	case FigureTrianlgeUp:
		line(x - size, y - size, x + size, y - size);
		line(x + size, y - size, x, y + size);
		line(x - size, y - size, x, y + size);
		break;
	case FigureTrianlge:
		line(x - size, y + size, x + size, y + size);
		line(x + size, y + size, x, y - size);
		line(x, y - size, x, y + size);
		break;
	case FigureClose:
		line(x - size, y - size, x + size, y + size);
		line(x - size, y + size, x + size, y - size);
		break;
	case FigureCross:
		line(x - size, y, x + size, y);
		line(x, y - size, x, y + size);
		break;
	default:
		break;
	}
}

void draw::paint(int x, int y, const char* name, figure_s type, int size) {
	if(!name)
		return;
	paint(x, y, type, size);
	text(x - textw(name) / 2, y + size + 2, name);
}

void draw::paint(int x, int y, figure_s type, color_s color, int size) {
	auto push = fore; fore = getcolor(color);
	paint(x, y, type, size);
	fore = push;
}

void draw::paint(int x, int y, const char* name, figure_s type, color_s color, int size) {
	auto push = fore; fore = getcolor(color);
	paint(x, y, name, type, size);
	fore = push;
}

bool draw::ishilite(int x, int y, int size, variant v) {
	auto r = ishilite({x - size, y - size, x + size, y + size});
	if(r)
		hilite_object = v;
	return r;
}

point draw::gethiliteback() {
	if(hot.hilite)
		return {-100, -100};
	return hot.mouse;
}

void draw::avatar(int x, int y, const char* id, color_s color, rect* rc_result, unsigned char alpha) {
	auto p = gres(id, "art/portraits");
	if(!p) {
		if(rc_result)
			rc_result->clear();
		return;
	}
	rect rc;
	rc.x1 = x - p->width / 2;
	rc.x2 = rc.x1 + p->width;
	rc.y1 = y - p->height / 2;
	rc.y2 = rc.y1 + p->height;
	image(rc.x1, rc.y1, p, 0, 0, alpha);
	rectb(rc, getcolor(color));
	if(rc_result)
		*rc_result = rc;
}

void draw::bar(rect rc, color_s color, color_s border, color_s back, int value, int maximum) {
	if(!value || !maximum)
		return;
	rect r1 = rc; r1.x1++; r1.y1++;
	if(value != maximum)
		r1.x2 = r1.x1 + (rc.width() - 2) * value / maximum;
	auto push = fore;
	if(back != NoColor) {
		fore = getcolor(back);
		rectf(rc);
	}
	fore = getcolor(color);
	rectf(r1);
	fore = getcolor(border);
	rectb(rc);
	fore = push;
}

bool draw::execute(const hotkey* source) {
	for(auto p = source; *p; p++) {
		if(hot.key == p->key) {
			p->proc();
			return true;
		}
	}
	return false;
}

HANDLER(after_initialize) {
	memset(&gui, 0, sizeof(gui));
	gui.window_width = 320;
	gui.border = 8;
	gui.padding = 4;
	gui.opacity = 186;
	gui.opacity_hilighted = 210;
	gui.hero_size = 64;
	gui.left_window_width = 800 - gui.window_width - gui.border * 4 - gui.padding - gui.border * 3;
	gui.tips_tab = 24;
	gui.grid = 32;
}