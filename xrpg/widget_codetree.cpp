#include "code.h"
#include "code_pack.h"
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
	pack* getcurrent() const {
		if(!getmaximum())
			return 0;
		return (pack*)get(current);
	}
	void update() {
		clear();
		for(auto& e : bsdata<pack>()) {
			if(!e)
				continue;
			addref(&e);
		}
		ensurevisible();
	}
	static const char* getpackname(const void* object, stringbuilder& sb) {
		return ((pack*)object)->getname();
	}
	void initialize() {
		auto& col = addcol("Name", {}, "Text");
		col.size = widtht::Auto;
		col.plist.getname = getpackname;
		show_header = false;
		readonly = true;
		no_change_count = true;
	}
} widget_codetypes_control;

static class widget_codemembers : public controls::tableref {
	pack* parent = 0;
	const char* getvalue(const char* id, stringbuilder& sb) const override {
		if(equal(id, "Name"))
			return getnm("CodeTreeMembers");
		return 0;
	}
	bool hastoolbar() const override {
		return false;
	}
public:
	void update() {
		clear();
		//for(auto& e : parent->symbols) {
		//	if(e.parent==This)
		//		addref(&e);
		//}
	}
	void paintnc() override {
		auto p = widget_codetypes_control.getcurrent();
		if(p != parent) {
			parent = p;
			update();
		}
		tableref::paintnc();
	}
	const char* getname(int line, int column, stringbuilder& sb) const override {
		auto& col = columns[column];
		//if(equal(col.id, "Name")) {
		//	auto p = (symbol*)get(line);
		//	return parent->getname(p->id);
		//} else
			return col.id;
	}
	void initialize() {
		auto& col = addcol("Name", {}, "CustomText").size = widtht::Auto;
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
	//auto p = widget_codetypes_control.getcurrent();
	//if(p)
	//	widget_codemembers_control.update(p->id);
}