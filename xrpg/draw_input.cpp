#include "draw.h"
#include "draw_button.h"
#include "draw_focus.h"
#include "handler.h"

static bool		break_modal;
static long		break_result;
static fnevent	next_proc;
extern rect		sys_static_area;
handler*		after_initialize;
handler*		after_input;
handler*		before_input;
handler*		before_modal;
handler*		leave_modal;

static void standart_domodal() {
	before_input->execute();
	draw::hot.key = draw::rawinput();
	if(!draw::hot.key)
		exit(0);
	after_input->execute();
}

bool draw::ismodal() {
	hot.cursor = CursorArrow;
	hot.hilite.clear();
	domodal = standart_domodal;
	before_modal->execute();
	if(hot.mouse.x < 0 || hot.mouse.y < 0)
		sys_static_area.clear();
	else
		sys_static_area = {0, 0, draw::getwidth(), draw::getheight()};
	if(!next_proc && !break_modal)
		return true;
	break_modal = false;
	leave_modal->execute();
	return false;
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

void draw::breakparam() {
	breakmodal(hot.param);
}

int draw::getresult() {
	return break_result;
}

void draw::cbsetint() {
	auto p = (int*)hot.object;
	*p = hot.param;
}

void draw::cbsetptr() {
	auto p = (long*)hot.object;
	*p = hot.param;
}

void draw::setnext(fnevent v) {
	next_proc = v;
}

void draw::application() {
	while(next_proc) {
		auto p = next_proc;
		next_proc = 0; p();
	}
}

void draw::initialize(const char* label, int timer) {
	colors::active = color(172, 128, 0);
	colors::border = color(73, 73, 80);
	colors::button = color(0, 38, 77);
	colors::form = color(32, 32, 32);
	colors::window = color(64, 64, 64);
	colors::text = color(255, 255, 255);
	colors::special = color(255, 244, 32);
	colors::border = color(0, 83, 166);
	colors::tips::text = color(255, 255, 255);
	colors::tips::back = color(100, 100, 120);
	colors::h1 = colors::text.mix(colors::button, 64);
	colors::h2 = colors::text.mix(colors::button, 96);
	colors::h3 = colors::text.mix(colors::button, 128);
	draw::font = metrics::font;
	draw::fore = colors::text;
	draw::fore_stroke = colors::blue;
	after_initialize->execute();
	create(-1, -1, 800, 600, 0, 32);
	setcaption(label);
	if(timer)
		settimer(timer);
}