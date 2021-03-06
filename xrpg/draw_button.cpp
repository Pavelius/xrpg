#include "crt.h"
#include "draw.h"
#include "draw_button.h"
#include "draw_figure.h"
#include "draw_focus.h"
#include "draw_simple.h"

using namespace draw;

long draw::getsource(void* source, int size) {
	if(size == sizeof(char))
		return *((char*)source);
	else if(size == sizeof(short))
		return *((short*)source);
	else if(size == sizeof(long))
		return *((long*)source);
	return 0;
}

void draw::setsource(void* source, int size, long value) {
	if(size == sizeof(char))
		*((char*)source) = (char)value;
	else if(size == sizeof(short))
		*((short*)source) = (short)value;
	else if(size == sizeof(long))
		*((long*)source) = (int)value;
}

const char* draw::getenumname(const void* object, stringbuilder& sb) {
	return getnm(*((const char**)object));
}

static void cbsetsource() {
	auto p = (void*)hot.object;
	auto s = hot.param2;
	auto v = hot.param;
	setsource(p, s, v);
}

void draw::setposition() {
	caret.x += metrics::padding;
	caret.y += metrics::padding;
	width -= metrics::padding * 2;
}

void draw::titletext(const char* label, int label_width, const char* separator) {
	if(!label || !label[0])
		return;
	if(!separator)
		separator = ":";
	char temp[1024]; stringbuilder sb(temp);
	sb.add(label);
	sb.add(separator);
	text(temp);
	caret.x += label_width;
	width -= label_width;
}

bool draw::addbutton(rect& rc, bool focused, const char* t1, unsigned k1, const char* tt1) {
	const int width = 18;
	rc.x2 -= width;
	auto result = button({rc.x2, rc.y1, rc.x2 + width, rc.y2},
		t1, tt1, k1, colors::button,
		false, false, false, false);
	auto push_fore = fore;
	fore = colors::border;
	caret.x = rc.x2;
	caret.y = rc.y1;
	line(rc.x2, rc.y2);
	fore = push_fore;
	return result;
}

int draw::addbutton(rect& rc, bool focused, const char* t1, unsigned k1, const char* tt1, const char* t2, unsigned k2, const char* tt2) {
	const int width = 20;
	rc.x2 -= width;
	auto height = rc.height() / 2;
	auto result = 0;
	if(button({rc.x2, rc.y1, rc.x2 + width, rc.y1 + height + 1},
		t1, tt1, k1, colors::button,
		false, false, false, false))
		result = 1;
	if(button({rc.x2, rc.y1 + height + 1, rc.x2 + width, rc.y1 + height * 2 + 1},
		t2, tt2, k2, colors::button,
		false, false, false, false))
		result = 2;
	if((hot.key == k2 || hot.key == k1) && !focused)
		result = 0;
	auto push_fore = fore;
	fore = colors::border;
	caret.x = rc.x2;
	caret.y = rc.y1;
	draw::line(rc.x2, rc.y2);
	fore = push_fore;
	return result;
}

bool draw::button(const rect& rc, const char* title, const char* tips, unsigned key, color value, bool focused, bool checked, bool press, bool border) {
	static rect rc_pressed;
	static int rc_key_event;
	auto push_fore = fore;
	bool result = false;
	bool vertical = rc.height() / 2 > rc.width();
	struct rect rcb = {rc.x1 + 1, rc.y1 + 1, rc.x2, rc.y2};
	auto a = ishilite(rcb);
	if((key && hot.key == key) || (focused && hot.key == KeyEnter) || (a && hot.key == MouseLeft && hot.pressed)) {
		rc_key_event = hot.key;
		rc_pressed = rc;
	}
	if((rc_pressed == rc) && rc_key_event == MouseLeft && !a) {
		rc_key_event = 0;
		rc_pressed.clear();
	}
	auto button_pressed = (rc_pressed == rc);
	if(button_pressed && (hot.key == InputKeyUp || (hot.key == MouseLeft && !hot.pressed))) {
		result = true;
		rc_key_event = 0;
		rc_pressed.clear();
	}
	if(checked)
		a = true;
	color c0 = value;
	if(a) {
		if(c0.gray().r >= 100)
			c0 = c0.mix(colors::black, 190);
		else
			c0 = c0.mix(colors::white, 190);
	}
	color b1 = c0;
	color b2 = c0.mix(colors::black);
	if(button_pressed) {
		if(vertical)
			gradh(b2, b1);
		else
			gradv(b2, b1);
	} else {
		if(vertical)
			gradh(b1, b2);
		else
			gradv(b1, b2);
	}
	if(border) {
		auto push_fore = fore;
		fore = focused ? colors::active : c0;
		if(fore.gray().r >= 100)
			fore = fore.mix(colors::black, 160);
		else
			fore = fore.mix(colors::white, 160);
		rectb();
		fore = push_fore;
	}
	auto rco = rc; rco.offset(2, 2);
	if(title) {
		rect r1 = rc;
		if(button_pressed)
			r1.y1 += 2;
		text(r1, title, AlignCenterCenter);
	}
	if(a && (tips || key) && !hot.pressed) {
		tooltips(tips);
		if(key) {
			char temp[64]; stringbuilder sb(temp); key2str(sb, key);
			tooltips(" (%HotKey: [%1])", temp);
		}
	}
	fore = push_fore;
	return result;
}

void draw::buttonr(int& x, int y, const char* label, fnevent proc, unsigned key) {
	if(!label || !proc)
		return;
	auto h = texth() + 8;
	auto w = textw(label) + 8;
	x -= w;
	rect r = {x, y, x + w, y + h};
	auto focus = isfocused(r, (void*)proc);
	auto result = button(r, label, 0, key, colors::button, focus, false, false, true);
	if(result)
		execute(proc);
	x -= metrics::padding;
}

void draw::buttonl(int& x, int y, const char* label, fnevent proc, unsigned key, void* focus_value) {
	if(!label || !proc)
		return;
	x += metrics::padding;
	y += metrics::padding;
	auto h = texth() + metrics::padding * 2;
	auto w = textw(label) + metrics::padding * 2;
	rect r = {x, y, x + w, y + h};
	if(!focus_value)
		focus_value = (void*)proc;
	auto focus = isfocused(r, focus_value);
	auto result = button(r, label, 0, key, colors::button, focus, false, false, true);
	if(result)
		execute(proc);
	x += w + metrics::padding;
}

void draw::radio(void* source, int size, unsigned bits, const char* label, const char* tips) {
	auto push_caret = caret;
	auto push_width = width;
	setposition();
	rect rc = {caret.x, caret.y, caret.x + width, caret.y};
	rect rc1 = {rc.x1 + 22, rc.y1, rc.x2, rc.y2};
	draw::textw(rc1, label);
	rc1.offset(-2);
	rc.y1 = rc1.y1;
	rc.y2 = rc1.y2;
	rc.x2 = rc1.x2;
	auto focused = isfocused(rc, source, bits);
	auto value = getsource(source, size);
	//auto checked = (value == bits);
	//clipart(x + 2, y + imax((rc1.height() - 14) / 2, 0), width, flags, ":radio");
	bool need_select = false;
	auto a = ishilite(rc);
	if(a && hot.key == MouseLeft) {
		if(!hot.pressed)
			need_select = true;
	}
	if(focused) {
		rectfocus();
		if(hot.key == KeySpace)
			need_select = true;
	}
	if(need_select)
		execute(cbsetsource, bits, size, source);
	rc = rc1; rc.offset(2);
	draw::text(rc, label);
	if(tips && a && !hot.pressed)
		tooltips(tips);
	width = push_width;
	caret = push_caret;
	caret.y += rc1.height() + 2;
}

static color getcolor(color normal, bool disabled) {
	if(disabled)
		return normal.mix(colors::window);
	return normal;
}

static void mark_check(int size, bool focused, bool checked, bool hilite) {
	auto push_fore = fore;
	fore = colors::window;
	if((hilite && hot.pressed))
		fore = fore.mix(colors::form);
	field(figure::RectFill, size);
	fore = colors::border;
	field(figure::Rect, size);
	fore = push_fore;
	if(checked) {
		auto push = linw; linw = 1.3;
		if(hilite)
			fore = fore.mix(colors::active, 32);
		field(figure::Check, size - 4);
		linw = push;
	}
	fore = push_fore;
}

void draw::checkbox(void* source, int size, unsigned bits, const char* label, const char* tips) {
	auto push_caret = caret;
	auto push_width = width;
	setposition();
	rect rc = {caret.x, caret.y, caret.x + width, caret.y};
	rect rc1 = {rc.x1 + 22, rc.y1, rc.x2, rc.y2};
	draw::textw(rc1, label);
	rc.y1 = rc1.y1;
	rc.y2 = rc1.y2;
	rc.x2 = rc1.x2;
	auto focused = isfocused(rc, source, bits);
	auto value = getsource(source, size);
	auto checked = (value & (1 << bits)) != 0;
	auto a = draw::ishilite(rc);
	caret.x += 10;
	caret.y += imax((rc1.height()) / 2, 0);
	mark_check(7, focused, checked, a);
	caret = push_caret;
	auto need_value = false;
	if(a && hot.key == MouseLeft) {
		if(!hot.pressed)
			need_value = true;
	}
	if(focused) {
		rectfocus();
		if(hot.key == KeySpace)
			need_value = true;
	}
	if(need_value)
		execute(cbsetsource, (value ^ (1 << bits)), size, source);
	draw::text(rc1, label);
	if(tips && a && !hot.pressed)
		tooltips(tips);
	caret = push_caret;
	width = push_width;
	caret.y += rc1.height() + metrics::padding;
}