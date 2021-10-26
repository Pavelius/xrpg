#include "answers.h"
#include "draw.h"
#include "draw_button.h"
#include "draw_simple.h"
#include "handler.h"

using namespace draw;

point				draw::camera;
int                 draw::grid_size = 32;
const void*			draw::hilite_object;
command*            draw::input_commands;
const char*         draw::image_url;
bool				draw::block_mode;
int                 draw::pausetime;
static point		camera_drag;
static rect			board;
static const void*	current_hilite;
extern stringbuilder tooltips_sb;

namespace metrics {
unsigned char			opacity = 230;
}

bool draw::swindow(bool hilight, int border) {
	if(border == 0)
		border = metrics::border;
	rectpush push;
	auto push_alpha = alpha;
	auto push_fore = fore;
	setoffset(-border, -border);
	auto rs = ishilite();
	alpha = metrics::opacity;
	fore = colors::form;
	if(hilight && rs) {
		fore = colors::button.mix(fore);
		if(hot.pressed)
			alpha = 0xFF;
	}
	rectf();
	alpha = push_alpha;
	fore = colors::border;
	rectb();
	fore = push_fore;
	return rs;
}

bool draw::ishilite(int s, const void* object) {
	if(!ishilite({caret.x - s, caret.y - s, caret.x + s, caret.y + s}))
		return false;
	current_hilite = object;
	return true;
}

void draw::stext(const char* string) {
	draw::link[0] = 0;
	textf(string);
	if(draw::link[0]) {
		if(draw::link[0] == '@') {
			variant v = (const char*)draw::link + 1;
			if(v)
				v.getinfo(tooltips_sb);
		} else
			tooltips(draw::link);
	}
}

bool draw::window(bool hilite, const char* string, const char* resid) {
	if(!string && !resid)
		return false;
	auto text_height = 0;
	auto image_height = 0;
	const sprite* image_surface = 0;
	if(string) {
		auto push_width = width;
		textfs(string);
		width = push_width;
		text_height = height;
	}
	if(resid) {
		image_surface = gres(resid, "art/images");
		if(image_surface)
			image_height = image_surface->get(0).sy;
	}
	auto padding_height = 0;
	if(image_height && text_height)
		padding_height = metrics::padding;
	auto push_height = height;
	height = image_height + text_height + padding_height;
	auto rs = swindow(hilite, 0);
	if(image_surface) {
		image(caret.x, caret.y, image_surface, 0, 0);
		caret.y += image_height + padding_height;
	}
	height = push_height;
	if(string)
		stext(string);
	caret.y += metrics::border * 2;
	return rs;
}

bool draw::buttonfd(const char* title) {
	if(!title)
		return false;
	auto push_width = width;
	textas(title);
	width = push_width;
	auto result = swindow(true, 0);
	texta(title, AlignCenterCenter);
	width = push_width;
	height += metrics::border * 2;
	return result;
}

bool draw::buttonrd(const char* title) {
	if(!title)
		return false;
	rect rc = {caret.x, caret.y, caret.x + width, caret.y};
	textw(rc, title);
	rc.y2 = rc.y2 + metrics::padding;
	auto push_height = height;
	height = rc.height();
	auto result = swindow(true, 0);
	height = push_height;
	text(rc, title, AlignCenterCenter);
	caret.x += rc.width() + metrics::padding * 3;
	return result;
}

void draw::answerbt(int i, long id, const char* title) {
	static char answer_hotkeys[] = {'1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I'};
	if(i >= (int)(sizeof(answer_hotkeys) / sizeof(answer_hotkeys[0])))
		i = sizeof(answer_hotkeys) / sizeof(answer_hotkeys[0]) - 1;
	if(button(title, answer_hotkeys[i], buttonfd))
		execute(buttonparam, id);
	if(control_hilited)
		hilite_object = (void*)id;
}

void draw::customwindow() {
	setposlu();
	auto height = 300;
	swindow(false, 0);
	tooltips(caret.x + metrics::padding, caret.y + height + metrics::padding * 4, width);
}

void draw::paintclear() {
	hilite_object = 0;
	auto push_fore = fore;
	fore = colors::window;
	rectf();
	fore = push_fore;
	tooltips(metrics::padding * 3, metrics::padding * 3, 320);
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
	setposld();
	windows(input_commands);
}

void draw::paintall() {
	paintclear();
	paintimage();
	paintcommands();
}

void draw::set(int x, int y) {
	caret.x = x - camera.x;
	caret.y = y - camera.y;
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
	setposru();
	if(columns == -1)
		columns = getcolumns(*this);
	auto column_width = 320;
	width = push_width;
	caret = push_caret;
	if(columns > 1)
		column_width = column_width / columns - metrics::padding;
	auto push_block = block_mode;
	block_mode = true;
	while(ismodal()) {
		if(pbackground)
			pbackground();
		if(pwindow)
			pwindow();
		auto push_caret = caret;
		auto push_width = width;
		setposru();
		window(false, title, resid);
		auto index = 0;
		auto y1 = caret.y, x1 = caret.x;
		auto y2 = caret.y;
		auto next_column = (elements.getcount() + columns - 1) / columns;
		width = column_width;
		auto push_padding = metrics::padding;
		metrics::padding = 0;
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
		metrics::padding = push_padding;
		caret.x = x1; caret.y = y2;
		width = push_width;
		if(cancel_text) {
			if(button(cancel_text, KeyEscape, buttonfd))
				execute(buttoncancel);
		}
		caret = push_caret;
		domodal();
	}
	block_mode = push_block;
	return getresult();
}

void draw::grid() {
	auto push_caret = caret;
	auto push_fore = fore;
	auto push_alpha = alpha;
	alpha = 64;
	fore = colors::border;
	auto x2 = getwidth(), y2 = getheight();
	auto sx = grid_size, sy = grid_size;
	for(auto x = 0; x < x2; x += sx) {
		setpos(x, 0);
		line(x, y2);
	}
	for(auto y = 0; y < y2; y += sy) {
		setpos(0, y);
		line(x2, y);
	}
	alpha = push_alpha;
	fore = push_fore;
	caret = push_caret;
}

void draw::avatar(const char* id) {
	auto p = gres(id, "art/portraits");
	if(!p)
		return;
	image(caret.x, caret.y, p, 0, 0);
	width = p->get(0).sx;
	rectb();
	caret.y += p->get(0).sy + 1;
}

void draw::bar(int value, int maximum) {
	if(!value || !maximum)
		return;
	int w2 = width;
	if(value != maximum)
		w2 = (width - 2) * value / maximum;
	auto push_fore = fore;
	auto push_width = width;
	fore = colors::form;
	rectf();
	fore = push_fore;
	width = w2;
	auto push_caret = caret;
	setoffset(1, 1);
	rectf();
	caret = push_caret;
	fore = colors::border;
	rectb();
	fore = push_fore;
	width = push_width;
	caret.y += height;
}

void draw::windows(const command* source) {
	for(auto p = source; *p; p++) {
		if(pwindow == p->proc)
			continue;
		if(button(getnm(p->id), p->key, buttonfd))
			execute(cbsetptr, (long)p->proc, 0, &pwindow);
	}
}

void set_dark_theme();

void tooltips_getrect();

static void paintcustomtips() {
	if(!draw::font)
		return;
	if(tooltips_sb && !tooltips_sb.begin()[0])
		return;
	rectpush push;
	tooltips_getrect();
	swindow(false, 0);
	auto push_fore = draw::fore;
	fore = colors::tips::text;
	textf(tooltips_sb.begin());
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