#include "crt.h"
#include "draw.h"
#include "draw_button.h"
#include "draw_control.h"
#include "draw_dockbar.h"
#include "io_plugin.h"
#include "setting.h"
#include "stringbuilder.h"

using namespace draw;
using namespace draw::controls;

bool						metrics::show::padding;
bool						metrics::show::bottom;
bool						metrics::show::top;
bool						metrics::show::left;
bool						metrics::show::right;

typedef adat<control*> controla;

BSDATA(docki) = {
	{"DockLeft"},
	{"DockLeftBottom"},
	{"DockRight"},
	{"DockRightBottom"},
	{"DockBottom"},
	{"DockWorkspace"},
};
assert_enum(docki, dock::Workspace)

plugin* plugin::first;

plugin::plugin(const char* id, dock position) : id(id), position(position), visible(true), next(0) {
	seqlink(this);
}

const plugin* plugin::find(const char* id) {
	for(auto p = first; p; p = p->next) {
		if(strcmp(p->id, id) == 0)
			return p;
	}
	return 0;
}

bool plugin::builder::canopen(const char* url) const {
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

static const char* get_control_name(const void* p, stringbuilder& sb) {
	auto pr = ((control*)p)->getvalue("Name", sb);
	if(!pr)
		pr = sb;
	return pr;
}

void getdocked(controla& result, dock type) {
	for(auto p = plugin::first; p; p = p->next) {
		if(!p->visible)
			continue;
		auto pc = p->getcontrol();
		if(!pc)
			continue;
		if(p->position == type)
			result.add(pc);
	}
}

static int paint_control(rect rc, const controla& elements, int& current, color back) {
	int y1 = rc.y1;
	if(current >= (int)elements.getcount())
		current = elements.getcount() - 1;
	auto& ec = *(elements.begin() + current);
	if(elements) {
		auto current_hilite = -1;
		const int dy = texth() + 8;
		auto push_fore = fore;
		fore = colors::border;
		if(metrics::show::padding)
			rectb({rc.x1, rc.y1 + dy, rc.x2, rc.y2});
		else
			line(rc.x1, rc.y1 + dy, rc.x2, rc.y1 + dy);
		fore = push_fore;
		rect rct = {rc.x1, rc.y1, rc.x2, rc.y1 + dy};
		if(tabs(rct, false, false, (void**)elements.begin(), 0, elements.getcount(),
			current, &current_hilite, get_control_name, 0, back)) {
			if(current_hilite != -1)
				current = current_hilite;
			else
				current = 0;
		}
		rc.y1 += dy;
		if(metrics::show::padding)
			rc.offset(metrics::padding);
		else
			rc.y1 += metrics::padding;
	}
	ec->view(rc, metrics::show::padding, true);
	return rc.y1 - y1;
}

static bool dock_paint(dock id, rect& client, const controla& p1, const controla& p2) {
	rect rc = client;
	auto& e1 = bsdata<docki>::elements[static_cast<int>(id)];
	auto& e2 = bsdata<docki>::elements[static_cast<int>(id) + 1];
	if(!p1 && !p2)
		return false;
	if(!e1.size)
		e1.size = 200;
	if(!e2.size)
		e2.size = 200;
	const int sx = metrics::padding + 2;
	switch(id) {
	case dock::Left:
		draw::splitv(rc.x1, rc.y1, e1.size, rc.height(), sx, 64, 400, false);
		client.x1 += e1.size + sx;
		rc.x2 = rc.x1 + e1.size;
		break;
	case dock::Right:
		draw::splitv(rc.x2, rc.y1, e1.size, rc.height(), sx, 64, 400, true);
		client.x2 -= e1.size + sx;
		rc.x1 = rc.x2 - e1.size;
		break;
	case dock::Bottom:
		draw::splith(rc.x1, rc.y2, rc.width(), e1.size, sx + 1, 64, 400, true);
		client.y2 -= e1.size + sx + 1;
		rc.y1 = rc.y2 - e1.size;
		break;
	default:
		return false;
	}
	if(!p2)
		paint_control(rc, p1, e1.current, colors::form);
	else if(!p1)
		paint_control(rc, p2, e2.current, colors::form);
	else if(id == dock::Left || id == dock::Right) {
		draw::splith(rc.x1, rc.y1, rc.width(), e2.size, sx, 64, 400, false);
		paint_control({rc.x1, rc.y1, rc.x2, rc.y1 + e2.size}, p1, e1.current, colors::form);
		paint_control({rc.x1, rc.y1 + e2.size + sx, rc.x2, rc.y2}, p2, e2.current, colors::form);
	}
	return true;
}

void draw::dockbar(rect& rc) {
	if(metrics::show::left) {
		controla p1; getdocked(p1, dock::Left);
		controla p2; getdocked(p2, dock::LeftBottom);
		dock_paint(dock::Left, rc, p1, p2);
	}
	if(metrics::show::right) {
		controla p1; getdocked(p1, dock::Right);
		controla p2; getdocked(p2, dock::RightBottom);
		dock_paint(dock::Right, rc, p1, p2);
	}
	if(metrics::show::bottom) {
		controla p1; getdocked(p1, dock::Bottom);
		dock_paint(dock::Bottom, rc, p1, {});
	}
}

static struct dockbar_settings_strategy : io::strategy {
	int getindex(const serializer::node& n) const {
		if(!n.parent)
			return -1;
		auto text = n.parent->name;
		for(auto& e : bsdata<docki>()) {
			if(equal(e.id, text))
				return &e - bsdata<docki>::elements;
		}
		return -1;
	}
	void write(serializer& file, void* param) override {
		for(auto& e : bsdata<docki>()) {
			char temp[32]; stringbuilder sb(temp);
			file.open(e.id);
			file.set("current", e.current);
			file.set("size", e.size);
			file.close(e.id);
		}
	}
	void set(serializer::node& n, const char* value) override {
		unsigned i = getindex(n);
		if(i == 0xFFFFFFFF)
			return;
		auto& e = bsdata<docki>::elements[i];
		if(n == "current")
			stringbuilder::read(value, e.current);
		else if(n == "size")
			stringbuilder::read(value, e.size);
	}
	dockbar_settings_strategy() : strategy("dockbar", "settings") {}
} dockbar_settings_strategy_instance;