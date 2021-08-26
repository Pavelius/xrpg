#include "crt.h"
#include "draw.h"
#include "draw_input.h"
#include "draw_focus.h"
#include "handler.h"
#include "io_plugin.h"

static bool		break_modal;
static long		break_result;
static fnevent	next_proc;
extern rect		sys_static_area;
handler*		after_initialize;
handler*		after_input;
handler*		before_initialize;
handler*		before_input;
handler*		before_modal;
handler*		leave_modal;
draw::awindowi	draw::awindow = {100, 100, 800, 600, 160, WFMinmax | WFResize};

static void standart_domodal() {
	before_input->execute();
	draw::hot.key = draw::rawinput();
	if(!draw::hot.key)
		exit(0);
	after_input->execute();
}

bool draw::ismodal() {
	hot.cursor = cursor::Arrow;
	hot.hilite.clear();
	if(hot.key == InputNeedUpdate)
		hot.key = InputUpdate;
	else
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

void draw::start() {
	while(next_proc) {
		auto p = next_proc;
		next_proc = 0; p();
	}
}

void draw::setneedupdate() {
	hot.key = InputNeedUpdate;
}

void draw::initialize(const char* title) {
	before_initialize->execute();
	after_initialize->execute();
	draw::font = metrics::font;
	draw::fore = colors::text;
	draw::fore_stroke = colors::blue;
	draw::create(awindow.x, awindow.y, awindow.width, awindow.height, awindow.flags, 32);
	draw::setcaption(title);
}