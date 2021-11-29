#include "crt.h"
#include "draw.h"
#include "main.h"

using namespace draw;

static unsigned long time_current;

namespace {
class effecti {
	point			position, start, finish;
	point*			save_position;
	const char*		text_message;
	color			text_color;
	unsigned long	time_start, time_finish;
public:
	explicit operator bool() const { return time_start < time_finish; }
	void clear() {
		memset(this, 0, sizeof(*this));
	}
	void setgoal(point v) {
		finish = v;
		start = position;
	}
	void setposition(point v) {
		position = v;
	}
	void stop() {
		setgoal(position);
	}
	void setmessage(const char* format, color format_color) {
		text_message = format;
		text_color = format_color;
	}
	void setduration(int duration) {
		time_start = time_current;
		time_finish = time_start + duration;
	}
	void apply_movement() {
		if(time_current < time_start)
			return;
		int dm = time_finish - time_start;
		if(!dm)
			return;
		int dc = time_current - time_start;
		if(dc > dm)
			dc = dm;
		position.x = (short)(start.x + (finish.x - start.x) * dc / dm);
		position.y = (short)(start.y + (finish.y - start.y) * dc / dm);
		if(save_position)
			*save_position = position;
	}
	void update() {
		apply_movement();
	}
	void paint() const {
		caret = position /* - draw::camera*/;
		if(!caret.in(clipping))
			return;
		fore = text_color;
		if(text_message) {
			auto push_caret = caret;
			caret.x -= textw(text_message) / 2;
			text(text_message);
			caret = push_caret;
		}
	}
	static bool ongoing() {
		for(auto& e : bsdata<effecti>()) {
			if(e.time_finish > time_current)
				return true;
		}
		return false;
	}
};
}

BSDATAC(effecti, 128)

static void update_effects() {
	for(auto& e : bsdata<effecti>()) {
		if(e)
			e.update();
	}
}

static void paint_effects() {
	auto push_caret = caret;
	auto push_fore = fore;
	for(auto& e : bsdata<effecti>()) {
		if(!e)
			continue;
		e.paint();
	}
	fore = push_fore;
	caret = push_caret;
}

void ui::fixbegin() {
	if(time_current < 1000)
		time_current = 1000;
}

void ui::fixend() {
	auto tms = getcputime();
	while(ismodal() && effecti::ongoing()) {
		auto tm = getcputime();
		if(tm > tms) {
			time_current += tm - tms;
			tms = tm;
		}
		update_effects();
		if(pbackground)
			pbackground();
		paint_effects();
		doredraw();
		waitcputime(1);
	}
}

static effecti* addnew() {
	for(auto& e : bsdata<effecti>()) {
		if(!e)
			return &e;
	}
	return (effecti*)bsdata<effecti>::source.add();
}

void ui::fixtext(point start, const char* format, color fore, int duration) {
	point pt = {0, -32};
	auto p = addnew();
	p->clear();
	p->setposition(start);
	p->setgoal(start + pt);
	p->setmessage(format, fore);
	p->setduration(duration);
}