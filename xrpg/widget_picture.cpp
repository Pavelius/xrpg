#include "draw.h"
#include "draw_button.h"
#include "draw_control.h"
#include "draw_focus.h"

using namespace draw;
using namespace draw::controls;

class picture_widget : public scrollable {
	surface			source;
	const char*		url;
	void paint() const override {
		rect rc = centerview(client);
		draw::blit(*draw::canvas, rc.x1, rc.y1, rc.width(), rc.height(), 0, source, origin_x, origin_y);
		if(isfocused())
			statuscolumn(0, 120, "%Size %1ix%2i", source.width, source.height);
	}
	const char* getvalue(const char* id, stringbuilder& sb) const override {
		if(equal(id, "URL"))
			return url;
		return 0;
	}
	void setvalue(const char* id, long value) override {
		if(equal(id, "URL"))
			url = szdup((const char*)value);
	}
	bool execute(const char* id, bool run) {
		if(equal(id, "Open")) {
			if(run) {
				source.read(url);
				setmaximum(source.width, source.height);
			}
		} else
			return false;
		return true;
	}
public:
	picture_widget() : source(), url(0) {}
};

static struct picture_plugin : plugin, plugin::builder {
	picture_plugin() : plugin("picture", dock::Bottom) {}
	control* getcontrol() {
		return 0;
	}
	plugin::builder* getbuilder() {
		return this;
	}
	control* create() override {
		return new picture_widget;
	}
	void getextensions(stringbuilder& sb) const override {
		for(auto pv = surface::plugin::first; pv; pv = pv->next) {
			sb.add("Изображение %+1 (%2)", pv->name, pv->filter);
			sb.addsz();
			sb.add(pv->filter);
			sb.addsz();
		}
	}
} picture_plugin_instance;

void initialize_picture() {
}