#include "draw.h"
#include "draw_button.h"
#include "draw_control.h"
#include "draw_focus.h"
#include "screenshoot.h"

using namespace draw;

static fnlist		current_list;
static const void*	current_object;
static array*		current_source;

bool controls::control::dropdown(const rect& rc) {
	screenshoot screen;
	pushfocus push_focus;
	setfocus(this, 0, true);
	while(ismodal()) {
		screen.restore();
		view(rc, true, true);
		rectf({rc.x2 + 1, rc.y1 + 4, rc.x2 + 5, rc.y2}, colors::black, 32);
		rectf({rc.x1 + 4, rc.y2, rc.x2 + 5, rc.y2 + 5}, colors::black, 32);
		domodal();
		switch(hot.key) {
		case KeyEscape:
			breakmodal(0);
			hot.key = InputNoUpdate;
			break;
		case KeyEnter:
			breakmodal(1);
			hot.key = InputNoUpdate;
			break;
		case KeyTab:
		case KeyTab | Shift:
		case F4:
			return true;
		case InputUpdate:
			// Leaving when lost focus or change windows size
			return false;
		case MouseLeft:
		case MouseLeft | Ctrl:
		case MouseLeft | Shift:
		case MouseLeftDBL:
		case MouseLeftDBL | Ctrl:
		case MouseLeftDBL | Shift:
			if(!ishilite(rc) && hot.pressed)
				return true;
			break;
		}
	}
	return 0;
}

struct combolist : public controls::list, public vector<void*> {
	const char* getname(int line, int column, stringbuilder& sb) const override {
		auto p = *((void**)ptr(line));
		return current_list.getname(p, sb);
	}
	static int compare_by_order(const void* v1, const void* v2) {
		char t1[256]; stringbuilder sb1(t1);
		char t2[256]; stringbuilder sb2(t2);
		auto p1 = *((void**)v1);
		auto p2 = *((void**)v2);
		auto n1 = current_list.getname(p1, sb1);
		auto n2 = current_list.getname(p2, sb2);
		return strcmp(n1, n2);
	}
	int	getmaximum() const {
		return getcount();
	}
//	void mouseselect(int id, bool pressed) override {
//		if(pressed)
//			list::mouseselect(id, pressed);
//		else
//			draw::execute(buttonok);
//	}
	int find(const void* object) const {
		for(auto p : *this) {
			if(p == object)
				return indexof(p);
		}
		return -1;
	}
	void sort() {
//		qsort(data, getcount(), sizeof(data[0]), compare_by_order);
	}
	void choose(const void* value) {
		current = find(value);
	}
//	static int getvalue(const anyval& var, const array& source) {
//		auto v = var.get();
//		if(var.getsize() != sizeof(void*))
//			v = (int)source.ptr(v);
//		return v;
//	}
	void update() {
		clear();
		auto pe = current_source->end();
		for(auto pb = current_source->begin(); pb < pe; pb += current_source->getsize()) {
			if(current_list.allow && !current_list.allow(current_object, (int)current_source->indexof(pb)))
				continue;
			add(pb);
		}
//		current = getvalue(cmb_var, cmb_source);
	}
};

static void post_combolist() {
	combolist list;
	list.update();
	if(list.getcount() == 0)
		return;
	list.hilite_odd_lines = false;
	list.sort();
	list.pixels_per_line = list.getrowheight();
	auto lines_per_page = imin(list.getcount(), (unsigned)7);
	rect rc = hot.focus;
	rc.x1 = rc.x1 - 1;
	rc.y1 = rc.y2;
	rc.y2 = rc.y1 + lines_per_page * list.pixels_per_line + 1;
	if(rc.y2 > getheight() - 2)
		rc.y2 = getheight() - 2;
	if(rc.y1 > rc.y2 - (list.pixels_per_line + 1))
		rc.y1 = rc.y2 - (list.pixels_per_line + 1);
//	list.choose(cmb_source.ptr(cmb_var.get()));
	list.ensurevisible();
	if(list.dropdown(rc)) {
//		auto p = list.data[list.current];
//		if(cmb_var.getsize() != sizeof(void*)) {
//			auto i = cmb_source.indexof(p);
//			cmb_var.set(i);
//		} else
//			cmb_var.set((int)p);
	}
}

static void dropdown(const rect& rc, void* source, int size, array& database, const void* object, const fnlist& plist, bool instant) {
	hot.focus = rc;
	current_list = plist;
	current_object = object;
	current_source = &database;
	if(instant)
		post_combolist();
	else
		execute(post_combolist);
}

static void fieldc(const rect& rc, unsigned flags, void* source, int size, array& database, const void* object, const fnlist& plist, const char* tips) {
	if(rc.width() <= 0)
		return;
	auto focused = isfocused(rc, source);
	auto a = ishilite(rc);
	color active = colors::button.mix(colors::active, 128);
	if(a) {
		if(hot.pressed)
			gradv(rc, active.darken(), active.lighten());
		else
			gradv(rc, active.lighten(), active.darken());
	} else
		gradv(rc, colors::button.lighten(), colors::button.darken());
	rectb(rc, colors::border);
	rect rco = rc + metrics::edit;
	if(focused)
		rectx(rco, colors::black);
	auto v = (void*)getsource(source, size);
	if(size != sizeof(void*))
		v = database.ptr((long)v);
	char temp[260]; stringbuilder sb(temp);
	auto pn = plist.getname(v, sb);
	if(pn)
		textc(rco.x1, rco.y1, rco.width(), pn);
	if(tips && a && !hot.pressed)
		tooltips(tips);
	auto execute_drop_down = false;
	if(a && hot.key == MouseLeft && !hot.pressed)
		execute_drop_down = true;
	if(focused && hot.key == KeyEnter)
		execute_drop_down = true;
	if(execute_drop_down)
		dropdown(rc, source, size, database, object, plist, false);
}

void draw::field(int x, int& y, int width, const char* label, void* source, int size, int label_width, array& database, const fnlist& plist, const char* tips) {
	if(!plist.getname)
		return;
	setposition(x, y, width);
	titletext(x, y, width, label, label_width);
	rect rc = {x, y, x + width, y + draw::texth() + metrics::edit * 2};
	fieldc(rc, AlignLeft | TextSingleLine, source, size, database, 0, plist, tips);
	y += rc.height() + metrics::padding;
}