#include "answers.h"
#include "draw.h"
#include "draw_button.h"
#include "draw_simple.h"
#include "handler.h"

using namespace draw;

point					draw::camera;
int                     draw::grid_size = 32;
variant                 draw::hilite_object;
command*                draw::input_commands;
const char*             draw::image_url;
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
	auto push_alpha = alpha;
	auto push_fore = fore;
	fore = c;
	alpha = op;
	rectf(rc);
	alpha = push_alpha;
	fore = b;
	rectb(rc);
	fore = push_fore;
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
	auto push_caret = caret;
	font = metrics::h2;
	fore = colors::h2;
	text(string);
	caret = push_caret;
	caret.y += texth() + 2;
	fore = push_fore;
	font = push_font;
}

void draw::stext(const char* string) {
	draw::link[0] = 0;
	textf(string);
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
		auto push_caret = caret; caret = {0, 0};
		draw::textf(string);
		text_height = caret.y;
		caret = push_caret;
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
	if(i >= (int)(sizeof(answer_hotkeys) / sizeof(answer_hotkeys[0])))
		i = sizeof(answer_hotkeys) / sizeof(answer_hotkeys[0]) - 1;
	bool is_hilited = false;
	if(button(title, answer_hotkeys[i], buttonfd, 0, &is_hilited))
		execute(breakparam, id);
	if(is_hilited)
		hilite_object = (void*)id;
}

void draw::customwindow() {
	setpositionlu();
	auto height = 300;
	swindow({caret.x, caret.y, caret.x + width, caret.y + height}, false, 0);
	tooltips(caret.x + metrics::padding, caret.y + height + metrics::padding * 4, width);
}

void draw::paintclear() {
	hilite_object.clear();
	auto push_fore = fore;
	fore = colors::window;
	rectf({0, 0, getwidth(), getheight()});
	tooltips(metrics::padding * 3, metrics::padding * 3, 320);
	fore = push_fore;
}

void draw::paintimage() {
	if(!image_url)
		return;
	auto p = gres(image_url, "art/background");
	if(!p)
		return;
	auto& fr = p->get(0);
	board.x1 = -camera.x;
	board.y1 = -camera.y;
	if(fr.sx < getwidth()) {
		board.x1 = (getwidth() - fr.sx) / 2;
		camera.x = -board.x1;
	}
	if(fr.sy < getheight()) {
		board.y1 = (getheight() - fr.sy) / 2;
		camera.y = -board.y1;
	}
	board.x2 = board.x1 + fr.sx;
	board.y2 = board.y1 + fr.sy;
	ishilite(board);
	image(board.x1, board.y1, p, 0, 0);
}

void draw::paintcommands() {
	if(!input_commands)
		return;
	setpositionld();
	windows(input_commands);
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

void draw::inputcamera() {
	const int step = 32;
	switch(hot.key) {
	case KeyLeft: execute(cbsetsht, camera.x - step, 0, &camera.x); break;
	case KeyRight: execute(cbsetsht, camera.x + step, 0, &camera.x); break;
	case KeyUp: execute(cbsetsht, camera.y - step, 0, &camera.y); break;
	case KeyDown: execute(cbsetsht, camera.y + step, 0, &camera.y); break;
	case MouseWheelUp: execute(cbsetsht, camera.y - step, 0, &camera.y); break;
	case MouseWheelDown: execute(cbsetsht, camera.y + step, 0, &camera.y); break;
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

static int getpassedtime(unsigned long start) {
	return getcputime() - start;
}

void draw::status(const char* format, ...) {
	if(!pausetime)
		return;
	auto start_time = getcputime();
	while(ismodal() && getpassedtime(start_time) < pausetime) {
		if(pbackground)
			pbackground();
		tooltips_sb.addv(format, xva_start(format));
		doredraw();
		waitcputime(1);
	}
}

long distance(point p1, point p2);

void draw::moving(point& result, point goal, int step) {
	auto start = result;
	auto maxds = distance(start, goal);
	auto curds = 0;
	while(ismodal() && curds < maxds) {
		result.x = (short)(start.x + (goal.x - start.x) * curds / maxds);
		result.y = (short)(start.y + (goal.y - start.y) * curds / maxds);
		if(pbackground)
			pbackground();
		doredraw();
		waitcputime(1);
		curds += step;
		if(curds > maxds)
			curds = maxds;
	}
	result = goal;
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

long answers::choose(const char* title, const char* cancel_text, bool interactive, const char* resid, int columns) const {
	if(!interactive)
		return random();
	if(!elements)
		return 0;
	auto push_caret = caret;
	auto push_width = width;
	setpositionru();
	if(columns==-1)
		columns = getcolumns(*this);
	auto column_width = 320;
	width = push_width;
	caret = push_caret;
	if(columns > 1)
		column_width = column_width / columns - metrics::padding;
	while(ismodal()) {
		if(pbackground)
			pbackground();
		if(pwindow)
			pwindow();
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
		caret = push_caret;
		//if(scene.doinput)
		//	scene.doinput();
		domodal();
	}
	return getresult();
}

void draw::grid() {
	auto push = fore;
	auto push_alpha = alpha;
	alpha = 64;
	fore = colors::border;
	auto x2 = getwidth(), y2 = getheight();
	auto sx = grid_size, sy = grid_size;
	for(auto x = 0; x < x2; x += sx)
		rectf({x, 0, x + 1, y2});
	for(auto y = 0; y < y2; y += sy)
		rectf({0, y, x2, y + 1});
	alpha = push_alpha;
	fore = push;
}

void draw::fog(int n) {
	auto push_alpha = alpha;
	auto push_fore = fore;
	alpha = n;
	fore = colors::form;
	rectf({caret.x, caret.y, caret.x + grid_size, caret.y + grid_size});
	fore = push_fore;
	alpha = push_alpha;
}

void draw::avatar(const char* id) {
	auto p = gres(id, "art/portraits");
	if(!p)
		return;
	image(caret.x, caret.y, p, 0, 0);
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
	auto push_fore = fore;
	fore = colors::form;
	rectf(rc);
	fore = push_fore;
	rectf(r1);
	fore = colors::border;
	rectb(rc);
	fore = push_fore;
	caret.y += rc.height();
}

static void choose_window() {
	pwindow = (fnevent)hot.object;
}

void draw::windows(const command* source) {
	for(auto p = source; *p; p++) {
		if(pwindow == p->proc)
			continue;
		if(buttonrd(getnm(p->id), p->key, getdescription(p->id)))
			execute(choose_window, 0, 0, (void*)p->proc);
	}
}

void set_dark_theme();

void tooltips_getrect(rect& rc, int border);

static void paintcustomtips() {
	if(!draw::font)
		return;
	if(tooltips_sb && !tooltips_sb.begin()[0])
		return;
	// Show background
	rect rc; tooltips_getrect(rc, 0);
	swindow(rc, false, 0);
	// Show text
	auto push_fore = draw::fore;
	auto push_width = width;
	auto push_caret = caret;
	fore = colors::tips::text;
	width = rc.width();
	caret.x = rc.x1; caret.y = rc.y1;
	textf(tooltips_sb.begin());
	caret = push_caret;
	width = push_width;
	fore = push_fore;
}

void draw::inputall() {
	inputcamera();
	paintcustomtips();
}

void draw::simpleinitialize() {
	set_dark_theme();
	if(!pbackground)
		pbackground = paintall;
	if(!ptips)
		ptips = inputall;
}