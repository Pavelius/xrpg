#include "crt.h"
#include "draw.h"
#include "draw_builder.h"
#include "draw_button.h"
#include "draw_control.h"
#include "draw_focus.h"
#include "handler.h"

using namespace draw;
using namespace draw::controls;

const sprite* control::std_images = (sprite*)loadb("art/pma/toolbar.pma");

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

bool control::isfocused() const {
	return draw::isfocused(this);
}

void control::post(const char* id) const {
	if(const_cast<control*>(this)->execute(id, false)) // Do not post command and do not clear hot.key if our command is disabled.
		draw::execute(command_execute, 0, (size_t)id, this);
}

void control::post(const char* id, int value) const {
	draw::execute(command_setvalue, value, (size_t)id, this);
}

void control::icon(int x, int y, int index, bool disabled) const {
	if(index != -1) {
		auto push_alpha = alpha;
		alpha = disabled ? 0x80 : 0xFF;
		image(x, y, getimages(), index, 0);
		alpha = push_alpha;
	}
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
		} else {
			auto key = getkey(id);
			if(!key || hot.key != key)
				continue;
			if(const_cast<control*>(p)->execute(id, false)) {
				p->post(id);
				return true;
			}
		}
	}
	return false;
}

void control::paint() const {
	auto focused = isfocusable() && draw::isfocused(client, this, 0, isfocused());
	if(ishilite(client)) {
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

void control::view(const rect& ro, bool show_border, bool show_background, bool show_toolbar, bool show_shadow) {
	rect rc = ro;
	if(show_toolbar && hastoolbar())
		rc.y1 += toolbar(rc.x1, rc.y1, rc.width(), 0);
	if(show_background) {
		auto push_fore = fore;
		fore = colors::window;
		rectf(rc);
		fore = push_fore;
	}
	if(show_border) {
		auto push_fore = fore;
		fore = colors::border;
		rectb(rc);
		fore = push_fore;
	}
	client = rc;
	paintnc();
	if(show_shadow) {
		auto push_fore = fore;
		auto push_alpha = alpha;
		fore = colors::form;
		alpha = 32;
		rectf({rc.x2 + 1, rc.y1 + 4, rc.x2 + 5, rc.y2});
		rectf({rc.x1 + 4, rc.y2, rc.x2 + 5, rc.y2 + 5});
		alpha = push_alpha;
		fore = push_fore;
	}
}