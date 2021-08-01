#include "crt.h"
#include "draw.h"
#include "draw_button.h"
#include "draw_focus.h"

using namespace draw;

int metrics::padding = 4;

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

static void cbsetsource() {
	auto p = (void*)hot.object;
	auto s = hot.param2;
	auto v = hot.param;
	setsource(p, s, v);
}

void draw::setposition(int& x, int& y, int& width, int padding) {
	if(padding == -1)
		padding = metrics::padding;
	x += padding;
	y += padding;
	width -= padding * 2;
}

void draw::titletext(int& x, int y, int& width, const char* label, int label_width, const char* separator) {
	if(!label || !label[0])
		return;
	if(!separator)
		separator = ":";
	char temp[1024]; stringbuilder sb(temp);
	sb.add(label);
	sb.add(separator);
	text(x, y + 4, temp);
	x += label_width;
	width -= label_width;
}

bool draw::addbutton(rect& rc, bool focused, const char* t1, unsigned k1, const char* tt1) {
	const int width = 18;
	rc.x2 -= width;
	auto result = button({rc.x2, rc.y1, rc.x2 + width, rc.y2},
		t1, tt1, k1, colors::button,
		false, false, false, false);
	line(rc.x2, rc.y1, rc.x2, rc.y2, colors::border);
	return result;
}

int draw::addbutton(rect& rc, bool focused, const char* t1, unsigned k1, const char* tt1, const char* t2, unsigned k2, const char* tt2) {
	const int width = 20;
	rc.x2 -= width;
	auto height = rc.height() / 2;
	auto result = 0;
	if(button({rc.x2, rc.y1, rc.x2 + width, rc.y1 + height},
		t1, tt1, k1, colors::button,
		false, false, false, false))
		result = 1;
	if(button({rc.x2, rc.y1 + height, rc.x2 + width, rc.y1 + height * 2},
		t2, tt2, k2, colors::button,
		false, false, false, false))
		result = 2;
	if((hot.key == k2 || hot.key == k1) && !focused)
		result = 0;
	draw::line(rc.x2, rc.y1, rc.x2, rc.y2, colors::border);
	return result;
}

bool draw::button(const rect& rc, const char* title, const char* tips, unsigned key, color value, bool focused, bool checked, bool press, bool border) {
	static rect rc_pressed;
	static int rc_key_event;
	auto push_fore = fore;
	bool result = false;
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
			c0 = c0.mix(colors::black, 160);
		else
			c0 = c0.mix(colors::white, 160);
	}
	color b1 = c0;
	color b2 = c0.mix(colors::black);
	if(button_pressed)
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
	auto h = texth() + 8;
	auto w = textw(label) + 8;
	rect r = {x, y, x + w, y + h};
	if(!focus_value)
		focus_value = (void*)proc;
	auto focus = isfocused(r, focus_value);
	auto result = button(r, label, 0, key, colors::button, focus, false, false, true);
	if(result)
		execute(proc);
	x += w + metrics::padding;
}

void draw::radio(int x, int& y, int width, void* source, int size, unsigned bits, const char* label, const char* tips) {
	setposition(x, y, width, 1);
	rect rc = {x, y, x + width, y};
	rect rc1 = {rc.x1 + 22, rc.y1, rc.x2, rc.y2};
	draw::textw(rc1, label);
	rc1.offset(-2);
	rc.y1 = rc1.y1;
	rc.y2 = rc1.y2;
	rc.x2 = rc1.x2;
	auto focused = isfocused(rc, source, bits);
	auto value = getsource(source, size);
	auto checked = (value == bits);
	//clipart(x + 2, y + imax((rc1.height() - 14) / 2, 0), width, flags, ":radio");
	bool need_select = false;
	auto a = ishilite(rc);
	if(a && hot.key == MouseLeft) {
		if(!hot.pressed)
			need_select = true;
	}
	if(focused) {
		rectx({rc1.x1, rc1.y1, rc1.x2, rc1.y2}, draw::fore);
		if(hot.key == KeySpace)
			need_select = true;
	}
	if(need_select)
		execute(cbsetsource, bits, size, source);
	rc = rc1; rc.offset(2);
	draw::text(rc, label);
	if(tips && a && !hot.pressed)
		tooltips(tips);
	y += rc1.height() + 2;
}

void draw::checkbox(int x, int& y, int width, void* source, int size, unsigned bits, const char* label, const char* tips) {
	setposition(x, y, width, 1);
	rect rc = {x, y, x + width, y};
	rect rc1 = {rc.x1 + 22, rc.y1, rc.x2, rc.y2};
	draw::textw(rc1, label);
	rc.y1 = rc1.y1;
	rc.y2 = rc1.y2;
	rc.x2 = rc1.x2;
	auto focused = isfocused(rc, source, bits);
	auto value = getsource(source, size);
	auto checked = (value & (1 << bits)) != 0;
	//	clipart(x + 2, y + imax((rc1.height() - 14) / 2, 0), 0, flags, ":check");
	auto a = draw::ishilite(rc);
	auto need_value = false;
	if(a && hot.key == MouseLeft) {
		if(!hot.pressed)
			need_value = true;
	}
	if(focused) {
		draw::rectx({rc1.x1 - 2, rc1.y1 - 1, rc1.x2 + 2, rc1.y2 + 1}, draw::fore);
		if(hot.key == KeySpace)
			need_value = true;
	}
	if(need_value)
		execute(cbsetsource, (value ^ (1 << bits)), size, source);
	draw::text(rc1, label);
	if(tips && a && !hot.pressed)
		tooltips(tips);
	y += rc1.height() + metrics::padding;
}