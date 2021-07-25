#include "answers.h"
#include "draw.h"
#include "variant.h"
#include "view.h"

using namespace draw;

static bool				break_modal;
static int				break_result;
static point			camera;
static point			camera_drag;
static rect				last_board;
static point			tooltips_point;
static short			tooltips_width;
static char				tooltips_text[4096];
fnevent					draw::domodal;
extern rect				sys_static_area;
guii					gui; template<> array bsdata<guii>::source(&gui, sizeof(gui), 1, 1);
static sprite*			small_font = (sprite*)loadb("art/fonts/small.pma");
static char				answer_hotkeys[] = {'1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};
point					draw::hilite_grid;
variant					draw::hilite_object;
formi					draw::form;
static fnevent			next_proc;
static void*			current_focus;

long					distance(point p1, point p2);

static color getcolor(color_s v) {
	static color theme_colors[] = {
		{235, 90, 70}, {97, 189, 79}, {0, 121, 191}, {242, 214, 0}, {255, 159, 26},
		{52, 69, 99}, {0, 194, 224}, {81, 232, 152},
		{0, 0, 0}, {255, 255, 255}, {179, 186, 197},
	};
	return theme_colors[v];
}

void draw::execute(fnevent proc, int value, int value2, void* object) {
	domodal = proc;
	hot.key = 0;
	hot.param = value;
	hot.param2 = value2;
	hot.object = object;
}

void draw::breakmodal(int result) {
	break_modal = true;
	break_result = result;
}

void draw::buttoncancel() {
	breakmodal(0);
}

void draw::buttonok() {
	breakmodal(1);
}

int draw::getresult() {
	return break_result;
}

void draw::breakparam() {
	breakmodal(hot.param);
}

static void keyparam() {
	hot.key = hot.param;
	hot.param = 0;
}

bool draw::window(rect rc, bool hilight, int border) {
	if(border == 0)
		border = gui.border;
	rc.offset(-border, -border);
	color c = colors::form;
	color b = colors::form;
	auto rs = ishilite(rc);
	auto op = gui.opacity;
	if(hilight && rs) {
		op = gui.opacity_hilighted;
		if(hot.pressed)
			op = op + op / 2;
	}
	rectf(rc, c, op);
	rectb(rc, b);
	return rs;
}

static int render_text(int x, int y, int width, const char* string) {
	draw::link[0] = 0;
	auto result = textf(x, y, width, string);
	if(draw::link[0])
		tooltips(x, y, width, draw::link);
	return result;
}

bool draw::window(int x, int& y, int width, bool hilite, const char* string, const char* resid) {
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
			hot.cursor = CursorAll;
			if(hot.mouse.x >= 0 && hot.mouse.y >= 0)
				camera = camera_drag + (dragmouse - hot.mouse);
			return true;
		}
		return false;
	}
	return true;
}

void control_standart() {
	if(control_board())
		return;
}

void draw::tooltips(int x1, int y1, int width, const char* format, ...) {
	tooltips_point.x = x1;
	tooltips_point.y = y1;
	tooltips_width = width;
	stringbuilder sb(tooltips_text);
	sb.addv(format, xva_start(format));
}

static void render_tooltips() {
	if(!tooltips_text[0])
		return;
	rect rc;
	rc.x1 = tooltips_point.x + tooltips_width + gui.border * 2 + gui.padding;
	rc.y1 = tooltips_point.y;
	rc.x2 = rc.x1 + gui.tips_width;
	rc.y2 = rc.y1;
	draw::textf(rc, tooltips_text, gui.tips_tab);
	if(rc.x2 > getwidth() - gui.border - gui.padding) {
		auto w = rc.width();
		rc.x1 = tooltips_point.x - gui.border * 2 - gui.padding - w;
		rc.x2 = rc.x1 + w;
	}
	// Correct border
	int height = draw::getheight();
	int width = draw::getwidth();
	if(rc.y2 >= height)
		rc.move(0, height - 2 - rc.y2);
	if(rc.x2 >= width)
		rc.move(width - 2 - rc.x2, 0);
	window(rc, false, 0);
	draw::fore = colors::tips::text;
	draw::textf(rc.x1, rc.y1, rc.width(), tooltips_text, 0, 0, 0, 0, gui.tips_tab);
	tooltips_text[0] = 0;
}

void guii::initialize() {
	memset(&gui, 0, sizeof(gui));
	gui.window_width = 320;
	gui.border = 8;
	gui.padding = 4;
	gui.opacity = 192;
	gui.opacity_hilighted = 96;
	gui.hero_size = 64;
	gui.tips_width = 320;
	gui.tips_tab = 24;
	gui.grid = 32;
}

void draw::initialize() {
	colors::active = color::create(172, 128, 0);
	colors::border = color::create(73, 73, 80);
	colors::button = color::create(0, 38, 77);
	colors::form = color::create(32, 32, 32);
	colors::window = color::create(64, 64, 64);
	colors::text = color::create(255, 255, 255);
	colors::special = color::create(255, 244, 32);
	colors::border = color::create(0, 83, 166);
	colors::tips::text = color::create(255, 255, 255);
	colors::tips::back = color::create(100, 100, 120);
	colors::h1 = colors::text.mix(colors::button, 64);
	colors::h2 = colors::text.mix(colors::button, 96);
	colors::h3 = colors::text.mix(colors::button, 128);
	draw::font = metrics::font;
	draw::fore = colors::text;
	draw::fore_stroke = colors::blue;
	gui.initialize();
	create(-1, -1, 800, 600, 0, 32);
	setcaption("Space 4X");
	settimer(100);
}

static void standart_domodal() {
	render_tooltips();
	hot.key = draw::rawinput();
	if(!hot.key)
		exit(0);
}

static void clear_hot() {
	hot.cursor = CursorArrow;
	hot.hilite.clear();
	hilite_object.clear();
	hilite_grid = {-100, -100};
	domodal = standart_domodal;
}

bool draw::ismodal() {
	clear_hot();
	if(hot.mouse.x < 0 || hot.mouse.y < 0)
		sys_static_area.clear();
	else
		sys_static_area = {0, 0, draw::getwidth(), draw::getheight()};
	if(next_proc) {
		break_modal = false;
		return false;
	}
	if(!break_modal)
		return true;
	break_modal = false;
	return false;
}

static void setwindow(int& x, int& y) {
	x = getwidth() - gui.window_width - gui.border * 2;
	y = gui.border * 2;
}

static void shadow(const rect& rc, color c) {
	auto op = gui.opacity;
	rectf(rc, c, op);
}

static void uptop_tooltips() {
	tooltips_point.x = 0;
	tooltips_point.y = gui.border * 2;
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
	if(hilite) {
		uptop_tooltips();
		stringbuilder sb(tooltips_text);
		sb.add(title);
		if(key) {
			sb.addn("[");
			sb.add("Горячая клавиша: ");
			draw::key2str(sb, key);
			sb.add("]");
		}
	}
	return x + width + gui.border;
}

static int status(int x, int y, int width, int value, const char* title) {
	char temp[32]; stringbuilder sb(temp);
	sb.add("%1i", value);
	return status(x, y, width, temp, title);
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
}

void formi::after() const {
	if(hilite_object) {
		uptop_tooltips();
		stringbuilder sb(tooltips_text);
		hilite_object.getinfo(sb);
	}
}

static void answer_button(int x, int& y, long id, const char* string, unsigned key) {
	auto text_height = 0;
	text_height = texth(string, gui.window_width);
	rect rc = {x, y, x + gui.window_width, y + text_height};
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

void draw::dialogul(int& x, int& y, int& w, const char* header) {
	x = gui.border * 2;
	y = gui.border * 2;
	w = getwidth() - gui.window_width - gui.border * 4 - gui.padding - x;
	auto h = getheight() / 2;
	window({x, y, x + w, y + h}, false, 0);
	if(header) {
		auto push_font = font;
		auto push_fore = fore;
		font = metrics::h3;
		fore = colors::h3;
		text(x, y, header);
		y += texth() + 4;
		fore = push_fore;
		font = push_font;
	}
}

long answers::choose(const char* title, const char* cancel_text, bool interactive, const char* resid) const {
	if(!interactive)
		return random();
	int x, y;
	while(ismodal()) {
		form.before();
		setwindow(x, y);
		window(x, y, gui.window_width, false, title, resid);
		y += gui.padding;
		auto index = 0;
		for(auto& e : elements) {
			auto i = imin(sizeof(answer_hotkeys) / sizeof(answer_hotkeys[0]), (size_t)index);
			answer_button(x, y, e.id, e.text, answer_hotkeys[i]);
		}
		if(cancel_text)
			answer_button(x, y, 0, cancel_text, KeyEscape);
		form.after();
		domodal();
		control_standart();
	}
	return getresult();
}

int draw::scene(fnevent input) {
	while(ismodal()) {
		form.before();
		form.after();
		domodal();
		control_standart();
		if(input)
			input();
	}
	return getresult();
}

void draw::application() {
	while(next_proc) {
		auto p = next_proc;
		next_proc = 0; p();
	}
}

void draw::setnext(fnevent v) {
	next_proc = v;
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

static bool button(const rect& rc, const char* title, const char* tips, unsigned key, color value, bool focused, bool checked, bool press, bool border) {
	auto push_fore = fore;
	bool result = false;
	struct rect rcb = {rc.x1 + 1, rc.y1 + 1, rc.x2, rc.y2};
	auto a = ishilite(rcb);
	if(key && hot.key == key)
		result = true;
	if(a && hot.key == MouseLeft && hot.pressed == press)
		result = true;
	if(checked)
		a = true;
	color c0 = value;
	if(a) {
		if(c0.gray().r >= 100)
			c0 = c0.mix(colors::black, 160);
		else
			c0 = c0.mix(colors::white, 160);
	}
	color b1 = c0;
	color b2 = c0.mix(colors::black);
	if(a && hot.pressed)
		gradv(rcb, b2, b1);
	else
		gradv(rcb, b1, b2);
	if(border) {
		auto bc = focused ? colors::active : c0;
		if(bc.gray().r >= 100)
			bc = bc.mix(colors::black, 160);
		else
			bc = bc.mix(colors::white, 160);
		rectb(rc, bc);
	}
	auto rco = rc; rco.offset(2, 2);
	if(focused)
		rectx(rco, fore);
	if(title) {
		rect r1 = rc;
		if(a && hot.pressed)
			r1.y1 += 2;
		text(r1, title, AlignCenterCenter);
	}
	if(a && (tips || key) && !hot.pressed) {
		uptop_tooltips();
		stringbuilder sb(tooltips_text);
		sb.add(tips);
		if(key) {
			sb.addn("Горячая клавиша: [");
			key2str(sb, key);
			sb.add("]");
		}
	}
	fore = push_fore; 
	return result;
}

void draw::buttonr(int& x, int y, const char* title, fnevent proc, unsigned key) {
	if(!title || !proc)
		return;
	auto h = texth() + 6;
	auto w = textw(title) + 8;
	x -= w;
	rect r = {x, y, x + w, y + h};
	auto result = button(r, title, 0, key, colors::button, false, false, false, true);
	if(result)
		execute(proc);
	x -= gui.padding;
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