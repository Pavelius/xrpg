//#include "main.h"
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
static surface			map_image;
static fnevent			background;
fnevent					draw::domodal;
extern rect				sys_static_area;
guii					gui; template<> array bsdata<guii>::source(&gui, sizeof(gui), 1, 1);
static sprite*			sprite_shields = (sprite*)loadb("art/sprites/shields.pma");
static sprite*			small_font = (sprite*)loadb("art/fonts/small.pma");
static char				answer_hotkeys[] = {'1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};
static const char*		background_bitmap;
static variant			hilite_object;
static fnevent			next_proc;
static void*			current_focus;

int						distance(point p1, point p2);
extern void				sleep(unsigned ms); // Set random seed

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

static void breakparam() {
	breakmodal(hot.param);
}

static void keyparam() {
	hot.key = hot.param;
	hot.param = 0;
}

static bool window(rect rc, bool hilight, int border) {
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

static bool window(int x, int& y, int width, bool hilite, const char* string, const char* resid) {
	if(!string && !resid)
		return false;
	auto text_height = 0;
	auto image_height = 0;
	const sprite* image_surface = 0;
	if(string) {
		state push;
		clipping.clear();
		text_height = draw::textf(0, 0, width, string);
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
		state push;
		clipping.clear();
		text_height = draw::textf(0, 0, width, string);
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
	colors::button = color::create(0, 122, 204);
	colors::form = color::create(32, 32, 32);
	colors::window = color::create(64, 64, 64);
	colors::text = color::create(255, 255, 255);
	colors::edit = color::create(38, 79, 120);
	colors::special = color::create(255, 244, 32);
	colors::border = colors::window.mix(colors::text, 128);
	colors::tips::text = color::create(255, 255, 255);
	colors::tips::back = color::create(100, 100, 120);
	colors::tabs::back = color::create(255, 204, 0);
	colors::tabs::text = colors::black;
	colors::h1 = colors::text.mix(colors::edit, 64);
	colors::h2 = colors::text.mix(colors::edit, 96);
	colors::h3 = colors::text.mix(colors::edit, 128);
	draw::font = metrics::font;
	draw::fore = colors::text;
	draw::fore_stroke = colors::blue;
	draw::setbackground(0);
	gui.initialize();
	create(-1, -1, 800, 600, 0, 32);
	setcaption("Space 4X");
	settimer(70);
}

static void standart_domodal() {
	render_tooltips();
	hot.key = draw::rawinput();
	if(!hot.key)
		exit(0);
}

bool draw::ismodal() {
	hilite_object.clear();
	domodal = standart_domodal;
	hot.cursor = CursorArrow;
	hot.hilite.clear();
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

static void variant_tips() {
	if(hilite_object) {
		uptop_tooltips();
		stringbuilder sb(tooltips_text);
		hilite_object.getinfo(sb);
	}
}

static void show_menu() {
}

static void status_panel(bool allow_pause) {
	//const int button_width = 64;
	//rect rc = {0, getheight() - texth() - gui.border * 2, 184 + gui.border * 3, getheight()};
	//if(allow_pause)
	//	rc.x2 += button_width + gui.border;
	//shadow(rc, colors::form);
	//auto x1 = rc.x1 + gui.border;
	//auto y1 = rc.y1 + gui.border;
	//x1 = status(x1, y1, 120, game.getdate().getname(), "Текущая дата");
	//auto player = game.getplayer();
	//if(player)
	//	x1 = status(x1, y1, 64, player->get(Credits), "Ваши кредиты");
}

static void static_image() {
	if(background_bitmap)
		image(0, 0, gres(background_bitmap, "art/background"), 0, 0);
	else
		rectf({0, 0, getwidth(), getheight()}, colors::gray);
}

static void render_map() {
	draw::state push;
	rect rc = {0, 0, getwidth(), getheight()};
	last_board = rc;
	ishilite(last_board); // Drag and drop analize this result
	int w = rc.width();
	int h = rc.height();
	int x1 = camera.x - w / 2;
	int y1 = camera.y - h / 2;
	int x2 = x1 + w;
	int y2 = y1 + h;
	point last_mouse = {
		(short)(hot.mouse.x - rc.x1 - rc.width() / 2 + camera.x),
		(short)(hot.mouse.y - rc.y1 - rc.height() / 2 + camera.y)};
	if(x1 < 0) {
		rc.x1 -= x1;
		x1 = 0;
	}
	if(x2 > map_image.width) {
		rc.x2 -= x2 - map_image.width;
		x2 = map_image.width;
	}
	if(y1 < 0) {
		rc.y1 -= y1;
		y1 = 0;
	}
	if(y2 > map_image.height) {
		rc.y2 -= y2 - map_image.height;
		y2 = map_image.height;
	}
	draw::rectf(last_board, colors::gray);
	if(rc.width() > 0 && rc.height() > 0)
		blit(*draw::canvas, rc.x1, rc.y1, rc.width(), rc.height(), 0, map_image, x1, y1);
}

void play_area() {
	render_map();
	int x = gui.border * 2, y = gui.border * 2;
}

void draw::setbackground(fnevent proc) {
	background = proc;
}

fnevent draw::getbackground() {
	return background;
}

void draw::setbitmap(const char* id) {
	background_bitmap = szdup(id);
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

long answers::choosev(const char* title, const char* cancel_text, bool interactive, const char* resid, bool portraits) const {
	int x, y;
	if(!interactive)
		return random();
	while(ismodal()) {
		static_image();
		if(background)
			background();
		status_panel(false);
		setwindow(x, y);
		if(portraits)
			windowp(x, y, gui.window_width, false, title, resid);
		else
			window(x, y, gui.window_width, false, title, resid);
		y += gui.padding;
		auto index = 0;
		for(auto& e : elements) {
			auto i = imin(sizeof(answer_hotkeys) / sizeof(answer_hotkeys[0]), (size_t)index);
			answer_button(x, y, e.id, e.text, answer_hotkeys[i]);
		}
		if(cancel_text)
			answer_button(x, y, 0, cancel_text, KeyEscape);
		variant_tips();
		domodal();
		control_standart();
	}
	return getresult();
}

void draw::scene(varianta& objects) {
	while(ismodal()) {
		static_image();
		if(background)
			background();
		for(auto& v : objects) {

		}
		status_panel(false);
		variant_tips();
		domodal();
		control_standart();
	}
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
	if(n==0xFF)
		rectf({x1, y1, x1 + gui.grid, y1 + gui.grid}, colors::black);
	else
		rectf({x1, y1, x1 + gui.grid, y1 + gui.grid}, colors::black, n);
}

void draw::figure(int x, int y, figure_s figure, int size) {
	switch(figure) {
	case FigureCircle:
		circle(x, y, size);
		break;
	case FigureRect:
		rectb({x - size, y - size, x + size, y + size});
		break;
	case FigureTrianlge:
		triangle({(short)(x - size), (short)(y + size)}, {(short)(x + size), (short)(y + size)}, {(short)x, (short)(y - size)}, fore);
		break;
	default:
		break;
	}
}