#include "crt.h"
#include "draw.h"
#include "draw_builder.h"
#include "draw_button.h"
#include "draw_control.h"
#include "draw_focus.h"
#include "handler.h"

using namespace draw;
using namespace draw::controls;

static const control*	hilite_control;
const sprite*			control::std_images = (sprite*)loadb("art/pma/toolbar.pma");

HANDLER(before_modal) {
	hilite_control = 0;
}

static void command_execute() {
	auto p = (control*)hot.object;
	auto n = (const char*)hot.param2;
	p->execute(n, true);
}

static void command_setvalue() {
	auto p = (control*)hot.object;
	auto v = hot.param;
	auto n = (const char*)hot.param2;
	p->setvalue(n, v);
}

static void open_context_menu() {
	auto p = (control*)hot.object;
	p->contextmenu(p->getcommands());
}

bool control::ishilited() const {
	return (this == hilite_control);
}

void control::post(const char* id) const {
	if(const_cast<control*>(this)->execute(id, false)) // Do not post command and do not clear hot.key if our command is disabled.
		draw::execute(command_execute, 0, (size_t)id, this);
}

void control::post(const char* id, int value) const {
	draw::execute(command_setvalue, value, (size_t)id, this);
}

void control::icon(int x, int y, int index, bool disabled) const {
	if(index!=-1)
		image(x, y, getimages(), index, 0, disabled ? 0x80 : 0xFF);
}

bool control::plugin::builder::canopen(const char* url) const {
	auto ext = szext(url);
	if(!ext)
		return false;
	ext--;
	char temp[1024 * 8]; stringbuilder sb(temp);
	getextensions(sb);
	sb.addsz();
	auto ps = zend(temp);
	while(ps[1]) {
		auto pe = ps + 1;
		if(szpmatch(ext, pe))
			return true;
		ps = zend(pe);
	}
	return false;
}

static bool execute_command_by_keyboard(const controls::control* p, const char** source) {
	if(!source)
		return false;
	if(!hot.key)
		return false;
	for(auto pc = source; *pc; pc++) {
		auto id = *pc;
		if(id[0] == '#') {
			if(execute_command_by_keyboard(p, p->getcommands(id + 1)))
				return true;
		}
		auto key = getkey(id);
		if(!key || hot.key != key)
			continue;
		if(const_cast<control*>(p)->execute(id, false)) {
			p->post(id);
			return true;
		}
	}
	return false;
}

void control::paint(const rect& rc) {
	client = rc;
	auto focused = isfocusable() && isfocused(rc, this);
	if(ishilite(rc)) {
		hilite_control = this;
		if(!focused) {
			if(isfocusable() && (hot.key == MouseLeft || hot.key == MouseRight) && !hot.pressed)
				setfocus(this, 0, false);
		} else {
			switch(hot.key) {
			case MouseRight:
				if(hot.pressed)
					draw::execute(open_context_menu, 0, 0, this);
				break;
			}
		}
	}
	if(focused)
		execute_command_by_keyboard(this, getcommands());
}

static bool isallow_proc(const void* object, const char* id) {
	return ((control*)object)->execute(id, false);
}

static const char** getcommands_proc(const void* object, const char* id) {
	return ((control*)object)->getcommands(id);
}

void control::contextmenu(const char** source) {
	auto id = draw::contextmenu(source, this, isallow_proc, getcommands_proc);
	if(id == 0)
		return;
	execute(id, true);
}

void control::view(const rect& ro, bool show_border, bool show_background, bool show_toolbar) {
	rect rc = ro;
	if(show_toolbar && hastoolbar())
		rc.y1 += toolbar(rc.x1, rc.y1, rc.width(), 0);
	if(show_background)
		rectf(rc, colors::window);
	if(show_border)
		rectb(rc, colors::border);
	paint(rc);
}