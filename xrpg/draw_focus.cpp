#include "crt.h"
#include "draw.h"
#include "draw_button.h"
#include "draw_focus.h"
#include "handler.h"

using namespace draw;

handler*				before_setfocus;

namespace {
struct focusable {
	rect				rc;
	const void*			value;
	unsigned			bits;
};
}

static bool				block_up_down;
static bool				block_left_right;
static const void*		current_focus;
static unsigned			current_bits;
static const void*		hilite_focus;
static unsigned			hilite_bits;
static adat<focusable>	elements;

HANDLER(before_modal) {
	elements.clear();
	hilite_focus = 0;
	hilite_bits = 0;
	block_left_right = false;
	block_up_down = false;
	hot.focus = {};
}

HANDLER(leave_modal) {
	setfocus(0, 0, true);
}

static void setfocus_callback() {
	setfocus(hot.object, hot.param, true);
}

static focusable* getby(const void* value, unsigned bits) {
	for(auto& e : elements) {
		if(e.value == value && e.bits == bits)
			return &e;
	}
	return 0;
}

static focusable* getfirst() {
	if(!elements)
		return 0;
	return elements.data;
}

static focusable* getlast() {
	if(!elements)
		return 0;
	return elements.end() - 1;
}

pushfocus::pushfocus() : focus(current_focus), bits(current_bits) {
}

pushfocus::~pushfocus() {
	current_focus = focus;
	current_bits = bits;
}

void draw::setfocusable(const rect& rc, const void* value, unsigned bits) {
	auto p = elements.add();
	p->rc = rc;
	p->value = value;
	p->bits = bits;
}

static const focusable* getnext(const focusable* pc, int key) {
	if(!key)
		return pc;
	if(!pc)
		pc = getfirst();
	if(!pc)
		return 0;
	auto pe = pc;
	auto pl = getlast();
	int inc = 1;
	if(key == KeyLeft || key == KeyUp || key == (KeyTab | Shift))
		inc = -1;
	while(true) {
		pc += inc;
		if(pc > pl)
			pc = elements.data;
		else if(pc < elements.begin())
			pc = pl;
		if(pe == pc)
			return pe;
		switch(key) {
		case KeyRight:
			if(pe->rc.y1 >= pc->rc.y1
				&& pe->rc.y1 <= pc->rc.y2
				&& pe->rc.x1 < pc->rc.x1)
				return pc;
			break;
		case KeyLeft:
			if(pe->rc.y1 >= pc->rc.y1
				&& pe->rc.y1 <= pc->rc.y2
				&& pe->rc.x1 > pc->rc.x1)
				return pc;
			break;
		case KeyDown:
			if(pc->rc.y1 >= pe->rc.y2)
				return pc;
			break;
		case KeyUp:
			if(pc->rc.y2 <= pe->rc.y1)
				return pc;
			break;
		default:
			return pc;
		}
	}
}

void draw::blockfocuskeys() {
	block_up_down = true;
	block_left_right = true;
}

bool draw::isfocused() {
	return static_cast<bool>(current_focus);
}

bool draw::isfocused(const void* value, unsigned bits) {
	return (current_focus == value) && (current_bits == bits);
}

bool draw::isfocused(const rect& rc, const void* value, unsigned bits, bool force_focused) {
	setfocusable(rc, value, bits);
	if(!force_focused && !isfocused())
		setfocus(value, bits, true);
	if(ishilite(rc)) {
		hilite_focus = value;
		hilite_bits = bits;
	}
	auto result = force_focused || isfocused(value, bits);
	if(result)
		hot.focus = rc;
	return result;
}

void draw::setfocus(const void* value, unsigned bits, bool instant) {
	if(isfocused(value, bits))
		return;
	if(instant) {
		if(current_focus != value || current_bits != bits)
			before_setfocus->execute();
		current_focus = value;
		current_bits = bits;
	} else
		execute(setfocus_callback, bits, 0, (void*)value);
}

HANDLER(after_input) {
	const focusable* p;
	if(block_left_right && (hot.key == KeyLeft || hot.key == KeyRight))
		return;
	if(block_up_down && (hot.key == KeyUp || hot.key == KeyDown))
		return;
	switch(hot.key) {
	case KeyTab:
	case KeyTab | Ctrl:
	case KeyUp:
	case KeyDown:
	case KeyLeft:
	case KeyRight:
		p = getnext(getby(current_focus, current_bits), hot.key);
		if(!p || (p->value==current_focus && p->bits==current_bits))
			return;
		setfocus(p->value, p->bits, true);
		break;
	case MouseLeft:
	case MouseRight:
		if(!hilite_focus || !hot.pressed)
			return;
		if(hilite_focus == current_focus && hilite_bits == current_bits)
			return;
		setfocus(hilite_focus, hilite_bits, true);
		return; // Mouse must be handled
	default: return;
	}
	hot.key = 0;
}