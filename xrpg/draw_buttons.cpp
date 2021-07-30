#include "crt.h"
#include "draw_control.h"
#include "draw_focus.h"

using namespace draw;

int metrics::padding = 4;

static long getsource(void* source, int size) {
	switch(size) {
	case 1: return *((char*)source);
	case 2: return *((short*)source);
	case 4: return *((int*)source);
	default: return 0;
	}
}

static void setsource(void* source, int size, long value) {
	switch(size) {
	case 1: *((char*)source) = (char)value; break;
	case 2: *((short*)source) = (short)value; break;
	case 4: *((int*)source) = (int)value; break;
	case 8: *((long long*)source) = value; break;
	default: break;
	}
}

static void setvalue() {
	auto p = (void*)hot.object;
	auto s = hot.param2;
	auto v = hot.param;
	setsource(p, s, v);
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

void draw::setposition(int& x, int& y, int& width, int padding) {
	if(padding == -1)
		padding = metrics::padding;
	x += padding;
	y += padding;
	width -= padding * 2;
}

void draw::titletext(int& x, int y, int& width, const char* label, int title, const char* separator) {
	if(!separator)
		separator = ":";
	char temp[1024]; stringbuilder sb(temp);
	sb.add(label);
	sb.add(separator);
	text(x, y + 4, temp);
	x += title;
	width -= title;
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
	auto checked = (value==bits);
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
		execute(setvalue, bits, size, source);
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
	auto checked = (value & bits) != 0;
//	clipart(x + 2, y + imax((rc1.height() - 14) / 2, 0), 0, flags, ":check");
//	decortext(flags);
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
		execute(setvalue, value^bits, size, source);
	draw::text(rc1, label);
	if(tips && a && !hot.pressed)
		tooltips(tips);
	y += rc1.height() + 2;
}