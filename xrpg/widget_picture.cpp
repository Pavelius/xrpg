#include "draw.h"
#include "draw_control.h"

using namespace draw;
using namespace draw::controls;

class picture_widget : public scrollable {
	surface			source;
	const char*		url;
	void redraw(const rect& rco) override {
		rect rc = centerview(rco);
		draw::blit(*draw::canvas, rc.x1, rc.y1, rc.width(), rc.height(), 0, source, origin_x, origin_y);
	}
	const char* getvalue(const char* id, stringbuilder& sb) const override {
		if(equal(id, "URL"))
			return url;
		return 0;
	}
public:
	picture_widget() : source(), url(0) {}
	picture_widget(const char* url) : source(url), url(szdup(url)) {
		setmaximum(source.width, source.height);
	}
};

static struct picture_plugin : control::plugin, control::plugin::builder {
	picture_plugin() : plugin("picture", dock::Bottom) {}
	control* getcontrol() {
		return 0;
	}
	control::plugin::builder* getbuilder() {
		return this;
	}
	control* create(const char* url) override {
		return new picture_widget(url);
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