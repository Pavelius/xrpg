#include "draw.h"
#include "draw_control.h"
#include "draw_builder.h"
#include "draw_button.h"
#include "draw_focus.h"

using namespace draw;

static bool tool(const rect& rc, bool disabled, bool checked, bool press, bool focused, int key = 0) {
	if(disabled)
		return false;
	bool result = false;
	rect rch = rc; rch.x2--; rch.y2--;
	auto a = ishilite(rch);
	auto pressed = hot.pressed;
	if(a && hot.key == MouseLeft && hot.pressed == press)
		result = true;
	if(focused && key && hot.key == key)
		result = true;
	if(checked)
		pressed = true;
	if(a) {
		if(pressed) {
			rectf(rc, colors::form.mix(colors::border, 128));
			rectb(rc, colors::border);

		} else {
			rectf(rc, colors::form.mix(colors::active, 128));
			rectb(rc, colors::border);
		}
	}
	return result;
}

struct toolbar_builder : builder {
	int		x, y, x2;
	point	size;
	constexpr toolbar_builder(int x, int y, int width, const point size) : builder(), x(x), y(y), x2(x + width), size(size) {
	}
	void addseparator() override {
		gradv({x + 3, y + 4, x + 5, y + size.y - 4}, colors::border.lighten(), colors::border);
		x += 7;
	}
	void add(const char* id) override {
		auto width = size.x;
		rect rc = {x, y, x + width, y + size.y};
		auto key = draw::getkey(id);
		if(ishilite(rc)) {
			auto name = getnm(id);
			if(name) {
				if(key) {
					char temp[128]; stringbuilder sb(temp); key2str(sb, key);
					tooltips("%1 (%2)", name, temp);
				} else
					tooltips(name);
				statusbar("%ExecuteCommand '%1'", name);
			}
		}
		auto disabled = allowid && !allowid(object, id);
		auto need_run = false;
		//if(draw::isfocused(object) && key && hot.key==key)
		//	need_run = true;
		if(tool(rc, disabled, false, true, false))
			need_run = true;
		if(need_run) {
			if(object)
				static_cast<const controls::control*>(object)->post(id);
		}
		if(object)
			static_cast<const controls::control*>(object)->icon(x + size.x / 2 + 1, y + size.y / 2 + 1, id, disabled);
		x += width;
	}
};

static bool isallow_proc(const void* object, const char* id) {
	return ((controls::control*)object)->execute(id, false);
}

static const char** getcommands_proc(const void* object, const char* id) {
	return ((controls::control*)object)->getcommands(id);
}

int	controls::control::toolbar(int x, int y, int width, int* next_x) const {
	auto commands = getcommands();
	if(next_x)
		*next_x = x;
	if(!commands)
		return 0;
	auto images = getimages();
	if(!images)
		return 0;
	short height = images->get(0).getrect(0, 0, 0).height() + 4;
	toolbar_builder e(x, y, width, {height, height});
	e.render(getcommands(), this, isallow_proc, getcommands_proc);
	if(next_x)
		*next_x = e.x;
	if(e.x != x)
		return height + 4;
	return 0;
}