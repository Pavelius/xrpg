#include "code.h"
#include "draw_button.h"
#include "draw_control.h"
#include "handler.h"

using namespace draw;
using namespace code;

static bool need_update = true;

static class widget_codetypes : public controls::tableref {
	const char* getvalue(const char* id, stringbuilder& sb) const override {
		if(equal(id, "Name"))
			return getnm("CodeTreeTypes");
		return 0;
	}
	bool hastoolbar() const override {
		return false;
	}
public:
	typei* getcurrent() const {
		if(!getmaximum())
			return 0;
		return (typei*)get(current);
	}
	void update() {
		clear();
		for(auto& e : bsdata<typei>()) {
			if(!e || !e.url)
				continue;
			addref(&e);
		}
		ensurevisible();
	}
	void initialize() {
		addcol("Name", ANREQ(typei, id), "Text").size = widtht::Auto;
		show_header = false;
		readonly = true;
		no_change_count = true;
	}
} widget_codetypes_control;

static class widget_codemembers : public controls::tableref {
	const char* cashe_type = 0;
	const char* getvalue(const char* id, stringbuilder& sb) const override {
		if(equal(id, "Name"))
			return getnm("CodeTreeMembers");
		return 0;
	}
	bool hastoolbar() const override {
		return false;
	}
public:
	void update(const char* parent) {
		clear();
		for(auto& e : bsdata<memberi>()) {
			if(!e)
				continue;
			if(strcmp(e.parent, parent) != 0)
				continue;
			addref(&e);
		}
	}
	void paintnc() override {
		auto p = widget_codetypes_control.getcurrent();
		if(p && p->id != cashe_type) {
			update(p->id);
			cashe_type = p->id;
		}
		tableref::paintnc();
	}
	void initialize() {
		addcol("Name", ANREQ(memberi, id), "Text").size = widtht::Auto;
		show_header = false;
		readonly = true;
		no_change_count = true;
	}
} widget_codemembers_control;

static class widget_codetypes_plugin : controls::plugin {
	controls::control* getcontrol() override {
		return &widget_codetypes_control;
	}
public:
	widget_codetypes_plugin() : plugin("CodeTreeTypes", dock::Left) {}
} widget_codetypes_plugin_instance;

static class widget_codemembers_plugin : controls::plugin {
	controls::control* getcontrol() override {
		return &widget_codemembers_control;
	}
public:
	widget_codemembers_plugin() : plugin("CodeTreeMembers", dock::LeftBottom) {}
} widget_codemembers_plugin_instance;

void initialize_codetree() {
	widget_codetypes_control.initialize();
	widget_codemembers_control.initialize();
}

void update_codetree() {
	widget_codetypes_control.update();
	auto p = widget_codetypes_control.getcurrent();
	if(p)
		widget_codemembers_control.update(p->id);
}