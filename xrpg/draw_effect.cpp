#include "crt.h"
#include "draw.h"
#include "draw_fix.h"

using namespace draw;

BSDATAC(uieffect, 128)

static unsigned time_current = 1000;

bool uieffect::isactive() const {
	return time_finish > time_current;
}

void uieffect::clear() {
	memset(this, 0, sizeof(*this));
	alpha_start = alpha_finish = alpha = 255;
}

void uieffect::setgoal(point v) {
	finish = v;
	start = position;
}

void uieffect::setgoalalpha(int v) {
	alpha_start = alpha;
	alpha_finish = v;
}

void uieffect::setlinked(point& v) {
	set(v);
	save_position = &v;
}

void uieffect::stop() {
	setgoal(position);
}

void uieffect::setduration(int duration) {
	time_start = time_current;
	time_finish = time_start + duration;
}

void uieffect::update() {
	if(time_current < time_start)
		return;
	int dm = time_finish - time_start;
	if(!dm)
		return;
	int dc = time_current - time_start;
	if(!dc)
		return;
	if(dc > dm)
		dc = dm;
	position.x = (short)(start.x + (finish.x - start.x) * dc / dm);
	position.y = (short)(start.y + (finish.y - start.y) * dc / dm);
	auto va = alpha_start + (alpha_finish - alpha_start) * dc / dm;
	if(va < 0)
		va = 0;
	else if(va > 255)
		va = 255;
	alpha = va;
	if(save_position)
		*save_position = position;
}

void uieffect::paint() const {
	caret = position  - draw::camera;
	if(!caret.in(clipping))
		return;
	draw::fore = fore;
	draw::alpha = alpha;
	if(format) {
		auto push_font = font;
		font = metrics::h1;
		char temp[260]; stringbuilder sb(temp);
		sb.add(format, value);
		auto push_caret = caret;
		caret.x -= textw(temp) / 2;
		caret.y -= texth();
		text(temp, -1, TextStroke);
		caret = push_caret;
		font = push_font;
	}
}

static void updateall() {
	for(auto& e : bsdata<uieffect>()) {
		if(e)
			e.update();
	}
}

static void paintall() {
	auto push_caret = caret;
	auto push_fore = fore;
	for(auto& e : bsdata<uieffect>()) {
		if(!e)
			continue;
		e.paint();
	}
	fore = push_fore;
	caret = push_caret;
}

static bool ongoing(const uieffect* p) {
	if(p)
		return p->isactive();
	for(auto& e : bsdata<uieffect>()) {
		if(e.isactive())
			return true;
	}
	return false;
}

void uieffect::clearall() {
	bsdata<uieffect>::source.clear();
}

static void fixwait(const uieffect* p) {
	auto tms = getcputime();
	while(ismodal() && ongoing(p)) {
		auto tm = getcputime();
		if(tm > tms) {
			time_current += tm - tms;
			tms = tm;
		}
		updateall();
		if(pbackground)
			pbackground();
		paintall();
		doredraw();
		waitcputime(1);
	}
}

void uieffect::wait() {
	fixwait(this);
}

void uieffect::waitall() {
	fixwait(0);
}

static uieffect* addnew() {
	for(auto& e : bsdata<uieffect>()) {
		if(!e)
			return &e;
	}
	return (uieffect*)bsdata<uieffect>::source.add();
}

uieffect* uieffect::add() {
	auto p = addnew();
	p->clear();
	return p;
}

uieffect* uieffect::add(point position, const char* format, color fore, int duration) {
	point pt = {0, -64};
	auto p = uieffect::add();
	p->set(position);
	p->setgoal(position + pt);
	p->set(format);
	p->set(fore);
	p->setduration(duration);
	return p;
}