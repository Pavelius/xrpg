#include "crt.h"
#include "draw_control.h"
#include "draw_focus.h"

using namespace draw;
using namespace draw::controls;

sprite* control::std_images = (sprite*)loadb("art/pma/toolbar.pma");

static void command_execute() {
	auto p = (control*)hot.object;
	auto n = (const char*)hot.param2;
	if(p->isallow(n))
		p->execute(n);
}

void control::post(const char* id) const {
	draw::execute(command_execute, 0, (long)id, this);
}

void control::icon(int x, int y, const char* id, bool disabled) const {
	auto i = getimage(id);
	if(i == -1)
		return;
	image(x, y, getimages(), i, 0, disabled ? 0x80 : 0xFF);
}

static void open_context_menu() {
	auto p = (control*)hot.param;
	p->contextmenu(p->getcommands());
}

void control::paint(const rect& rc) const {
	auto focused = isfocusable() && isfocused(rc, this);
	if(ishilite(rc)) {
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
}

void builder::render(const control* parent, const char** commands, bool& separator, int& count) {
	if(!commands)
		return;
	for(auto p = commands; *p; p++) {
		auto id = *p;
		if(id && id[0] == '#') {
			id = id + 1;
			if(!parent->isallow(id))
				continue;
			separator = true;
			render(parent, parent->getcommands(id), separator, count);
			separator = true;
		} else {
			if(separator) {
				if(count > 0)
					addseparator();
				separator = false;
			}
			add(parent, *p);
			count++;
		}
	}
}

void builder::render(const control* parent, const char ** commands) {
	bool separator = false;
	auto count = 0;
	render(parent, commands, separator, count);
}

void control::contextmenu(const char** source, builder& pm) {
	if(!source)
		return;
	pm.start();
	pm.render(this, source);
	auto id = pm.finish();
	if(id) {
		if(isallow(id))
			execute(id);
	}
}

const char** control::getcommands(const char* id) const {
	static const char* cmd_edit[] = {"Cut", "Copy", "Paste", 0};
	if(equal(id, "Edit"))
		return cmd_edit;
	return 0;
}

const char* control::commands_edit[] = {"Cut", "Copy", "Paste", 0};