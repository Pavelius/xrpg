#include "draw.h"
#include "draw_button.h"
#include "draw_scroll.h"

using namespace draw;

static int					drag_value;
static draw::scroll::proc	call_proc;
static draw::scroll			call_object;
int							metrics::scroll = 16;

scroll::scroll(int& origin, int page, int maximum, const rect& client, bool horizontal, int wheel) :
	origin(&origin), page(page), maximum(maximum), wheel(wheel), client(client), horizontal(horizontal) {
	if(maximum > page) {
		if(horizontal)
			work.set(client.x1, client.y2 - metrics::scroll, client.x2, client.y2);
		else
			work.set(client.x2 - metrics::scroll, client.y1, client.x2, client.y2);
	} else
		work.clear();
}

bool scroll::ishilite() const {
	return draw::ishilite(work);
}

void scroll::correct() {
	if(!origin)
		return;
	if(*origin + page > maximum)
		*origin = maximum - page;
	if(*origin < 0)
		*origin = 0;
}

void scroll::callback() {
	(call_object.*call_proc)(hot.param);
}

void scroll::execute(draw::scroll::proc p, int param) const {
	call_proc = p;
	call_object = *this;
	draw::execute(callback, param);
}

rect scroll::getslide() const {
	if(!origin)
		return {0, 0, 0, 0};
	if(horizontal) {
		auto pix_page = work.width();
		auto ss = (pix_page * page) / maximum; // scroll size (in pixels)
		auto ds = pix_page - ss;
		auto dr = maximum - page;
		auto p = ((*origin)*ds) / dr + work.x1;
		return {p, work.y1, p + ss, work.y2};
	} else if(maximum) {
		auto pix_page = work.height();
		auto ss = (pix_page * page) / maximum; // scroll size (in pixels)
		auto ds = pix_page - ss;
		auto dr = maximum - page;
		auto p = ((*origin)*ds) / dr + work.y1;
		return {work.x1, p, work.x2, p + ss};
	} else
		return {work.x1, work.y1, work.x2, work.y1};
}

void scroll::view(bool focused) {
	if(!isvisible())
		return;
	auto a = !dragactive() && hot.mouse.in(work);
	if(dragactive(origin))
		a = true;
	if(a) {
		auto slide = getslide();
		auto push_alpha = alpha;
		auto push_fore = fore;
		alpha = 128;
		fore = colors::button;
		rectf();
		alpha = push_alpha;
		fore = push_fore;
		button(slide, 0, 0, 0, colors::button, false, hot.pressed, false, true);
	} else {
		if(focused) {
			auto slide = getslide();
			auto push_alpha = alpha;
			auto push_fore = fore;
			alpha = 128;
			fore = colors::active;
			if(horizontal)
				rectf();
			else
				rectf();
			alpha = push_alpha;
			fore = push_fore;
		}
	}
	// Mouse wheel is overlapped on window, so must be in view() block, rather that input()
	switch(hot.key) {
	case MouseWheelUp:
		if(draw::ishilite(client))
			execute(&scroll::setorigin, *origin - wheel);
		break;
	case MouseWheelDown:
		if(draw::ishilite(client))
			execute(&scroll::setorigin, *origin + wheel);
		break;
	}
}

void scroll::input() {
	if(!isvisible())
		return;
	if(dragactive(origin)) {
		int p1, pix_page;
		if(horizontal) {
			p1 = hot.mouse.x - drag_value - work.x1;
			pix_page = work.width();
		} else {
			p1 = hot.mouse.y - drag_value - work.y1;
			pix_page = work.height();
		}
		auto pix_scroll = (pix_page * page) / maximum; // scroll size (in pixels)
		auto dr = maximum - page;
		auto ds = pix_page - pix_scroll;
		*origin = (p1*dr) / ds;
		correct();
		//execute(&scroll::setorigin, (p1*dr) / ds);
	} else {
		switch(hot.key) {
		case MouseLeft:
			if(ishilite()) {
				if(hot.pressed) {
					auto slider = getslide();
					if(horizontal) {
						if(hot.mouse.x < slider.x1)
							execute(&scroll::setorigin, *origin - page);
						else if(hot.mouse.x > slider.x2)
							execute(&scroll::setorigin, *origin + page);
						else {
							dragbegin(origin);
							drag_value = hot.mouse.x - slider.x1;
						}
					} else {
						if(hot.mouse.y < slider.y1)
							execute(&scroll::setorigin, *origin - page);
						else if(hot.mouse.y > slider.y2)
							execute(&scroll::setorigin, *origin + page);
						else {
							dragbegin(origin);
							drag_value = hot.mouse.y - slider.y1;
						}
					}
				}
				hot.key = InputNoUpdate;
			}
			break;
		case MouseMove:
		case InputIdle:
		case MouseRight:
		case MouseLeftDBL:
			if(ishilite())
				hot.key = InputNoUpdate;
			break;
		case MouseWheelUp:
		//	if(draw::ishilite(client))
		//		execute(&scroll::setorigin, *origin - wheel);
			break;
		case MouseWheelDown:
		//	if(draw::ishilite(client))
		//		execute(&scroll::setorigin, *origin + wheel);
			break;
		}
	}
}