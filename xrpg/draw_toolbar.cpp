#include "draw_control.h"

using namespace draw;
using namespace draw::controls;

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
			rectf(rc, colors::active);
			rectb(rc, colors::border);

		} else {
			rectf(rc, colors::active.mix(colors::window, 128));
			rectb(rc, colors::border);
		}
	}
	return result;
}

struct toolbar_builder : builder {
	int		x, y, x2;
	point	size;
	constexpr toolbar_builder(int x, int y, int width, const point size) : x(x), y(y), x2(x + width), size(size) {}
	void addseparator() override {
		gradv({x + 3, y + 4, x + 5, y + size.y - 4}, colors::border.lighten(), colors::border);
		x += 7;
	}
	void add(const control* parent, const char* id) override {
		auto width = size.x;
		rect rc = {x, y, x + width, y + size.y};
		if(ishilite(rc)) {
			auto name = getnm(id);
			if(name) {
				auto key = 0;
				if(key) {
					char temp[128]; stringbuilder sb(temp); key2str(sb, key);
					tooltips("%1 (%2)", name, temp);
				} else
				tooltips(name);
				statusbar("%ExecuteCommand '%1'", name);
			}
		}
		auto disabled = !parent->isallow(id);
		if(tool(rc, disabled, false, true, false))
			parent->post(id);
		parent->icon(x + size.x / 2, y + size.y / 2, id, disabled);
		x += width;
	}
};

int	control::toolbar(int x, int y, int width, int* next_x) const {
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
	e.render(this, getcommands());
	if(next_x)
		*next_x = e.x;
	if(e.x != x)
		return height + 4;
	return 0;
}