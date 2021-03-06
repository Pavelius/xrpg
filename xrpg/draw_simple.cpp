#include "answers.h"
#include "draw.h"
#include "draw_button.h"
#include "draw_simple.h"
#include "handler.h"

using namespace draw;

int                 draw::grid_size = 32;
const void*			draw::hilite_object;
int                 draw::pausetime;
static point		camera_drag;
static rect			board;

namespace metrics {
unsigned char		opacity = 230;
}

bool draw::swindow(bool hilight) {
	rectpush push;
	auto push_alpha = alpha;
	auto push_fore = fore;
	setoffset(-metrics::border, -metrics::border);
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

bool draw::ishilite(int size, const void* object) {
	if(!ishilite({caret.x - size, caret.y - size, caret.x + size, caret.y + size}))
		return false;
	hilite_object = object;
	return true;
}

bool draw::window(bool hilite, const char* string, const char* resid) {
	if((!string || string[0] == 0) && !resid)
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
	auto rs = swindow(hilite);
	if(image_surface) {
		image(caret.x, caret.y, image_surface, 0, 0);
		caret.y += image_height + padding_height;
	}
	height = push_height;
	if(string)
		textf(string);
	caret.y += metrics::border * 2;
	return rs;
}

bool draw::buttonfd(const char* title) {
	if(!title)
		return false;
	auto push_width = width;
	textas(title);
	width = push_width;
	auto result = swindow(true);
	texta(title, AlignCenterCenter);
	width = push_width;
	height += metrics::border * 2;
	return result;
}

bool draw::buttonft(const char* title) {
	if(!title)
		return false;
	auto push_caret = caret;
	auto push_width = width;
	textfs(title);
	width = push_width;
	auto result = swindow(true);
	textf(title);
	width = push_width;
	height += metrics::border * 2;
	caret = push_caret;
	return result;
}

bool draw::buttonrd(const char* title) {
	if(!title)
		return false;
	auto push_caret = caret;
	auto push_height = height;
	textfs(title);
	auto result = swindow(true);
	textf(title);
	height = push_height;
	width += metrics::border * 2;
	caret = push_caret;
	return result;
}

void draw::texth2w(const char* string) {
	auto push_font = font;
	auto push_height = height;
	font = metrics::h2;
	height = texth();
	swindow(false);
	auto push_fore = fore;
	fore = colors::h2;
	texta(string, AlignCenter);
	fore = push_fore;
	caret.y += texth() + metrics::border * 2 + metrics::padding;
	height = push_height;
	font = push_font;
}

void draw::answerbt(int i, const void* pv, const char* title) {
	static char answer_hotkeys[] = {'1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I'};
	if(i >= (int)(sizeof(answer_hotkeys) / sizeof(answer_hotkeys[0])))
		i = sizeof(answer_hotkeys) / sizeof(answer_hotkeys[0]) - 1;
	auto proc = buttonfd;
	if(title[0] == '#') {
		title++;
		proc = buttonft;
	}
	if(button(title, answer_hotkeys[i], proc))
		execute(buttonparam, (long)pv);
	if(control_hilited) {
		if(answers::show_tips)
			hilite_object = pv;
	} else if(hilite_object) {
		if(pv == hilite_object) {
			hot.cursor = cursor::Hand;
			if(hot.key==MouseLeft && !hot.pressed)
				execute(buttonparam, (long)pv);
		}
	}
}

void draw::customwindow() {
	setposlu();
	auto height = 300;
	swindow(false);
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
		tips_sb.addv(format, xva_start(format));
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

void* answers::choose(const char* title, const char* cancel_text, bool interactive, const char* resid, int columns, const char* header) const {
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
		column_width = column_width / columns - metrics::border;
	while(ismodal()) {
		if(pbackground)
			pbackground();
		if(pwindow)
			pwindow();
		auto push_caret = caret;
		auto push_width = width;
		setposru();
		if(header)
			texth2w(header);
		if(beforepaint)
			beforepaint();
		window(false, title, resid);
		auto index = 0;
		auto y1 = caret.y, x1 = caret.x;
		auto y2 = caret.y;
		auto next_column = (elements.getcount() + columns - 1) / columns;
		auto push_width_normal = width;
		width = column_width;
		auto push_padding = metrics::padding;
		metrics::padding = 0;
		for(auto& e : *this) {
			answerbt(index, e.value, e.text);
			if(caret.y > y2)
				y2 = caret.y;
			if((index % next_column) == next_column - 1) {
				caret.y = y1;
				caret.x += column_width + metrics::border * 2;
			}
			index++;
		}
		metrics::padding = push_padding;
		caret.x = x1; caret.y = y2;
		width = push_width_normal;
		if(cancel_text) {
			if(button(cancel_text, KeyEscape, buttonfd))
				execute(buttoncancel);
		}
		width = push_width;
		if(afterpaint)
			afterpaint();
		caret = push_caret;
		domodal();
	}
	return (void*)getresult();
}

void draw::grid() {
	auto push_caret = caret;
	auto push_fore = fore;
	auto push_alpha = alpha;
	alpha = 64;
	fore = colors::border;
	auto x1 = (camera.x / draw::grid_size) * draw::grid_size - camera.x;
	auto y1 = (camera.y / draw::grid_size) * draw::grid_size - camera.y;
	auto x2 = getwidth(), y2 = getheight();
	auto sx = grid_size, sy = grid_size;
	for(auto x = x1; x < x2; x += sx) {
		setpos(x, 0);
		line(x, y2);
	}
	for(auto y = y1; y < y2; y += sy) {
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

void draw::fog(int n) {
	auto push_alpha = alpha;
	alpha = n;
	rectf();
	alpha = push_alpha;
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

static void tooltips_paint() {
	if(!draw::font)
		return;
	if(!tips_sb)
		return;
	rectpush push;
	tipspos();
	swindow(false);
	auto push_fore = draw::fore;
	fore = colors::tips::text;
	textf(tips_sb);
	fore = push_fore;
}

void draw::simpleui::tips() {
	tooltips_paint();
}

void draw::simpleui::beforemodal() {
	hilite_object = 0;
	tips_caret.x = metrics::padding + metrics::border;
	tips_caret.y = metrics::padding + metrics::border;
	tips_size.x = 320;
}

void draw::simpleui::paint() {
	auto push_fore = fore;
	fore = colors::window;
	rectf();
	fore = push_fore;
}