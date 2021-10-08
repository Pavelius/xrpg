#include "draw.h"
#include "draw_button.h"
#include "draw_control.h"
#include "draw_input.h"
#include "draw_focus.h"
#include "screenshoot.h"

using namespace draw;

static fnlist		current_list;
static const void*	current_object;
static array*		current_database;
static int			current_size;
static void*		current_source;

bool controls::control::dropdown(const rect& rc) {
	screenshoot screen;
	pushfocus push_focus;
	setfocus(this, 0, true);
	view(rc, true, true); // Calculate position and rect only
	opening(); // Some special actions when open control
	while(ismodal()) {
		screen.restore();
		view(rc, true, true, true, true);
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
	return getresult()!=0;
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

struct combolist : public controls::list, vector<void*> {
	const char* getname(int line, int column, stringbuilder& sb) const override {
		auto p = *((void**)ptr(line));
		return current_list.getname(p, sb);
	}
	int	getmaximum() const {
		return getcount();
	}
	void opening() {
		ensurevisible();
	}
	void mouseclick() const override {
		if(hot.pressed)
			list::mouseclick();
		else
			draw::execute(buttonok);
	}
	int find(const void* object) const {
		for(auto p : *this) {
			if(p == object)
				return indexof(p);
		}
		return -1;
	}
	const void* getvalue() const {
		if(current == -1)
			return 0;
		return *((void**)ptr(current));
	}
	void sort() {
		qsort(begin(), getcount(), sizeof(begin()[0]), compare_by_order);
	}
	void setvalue(const void* value) {
		current = find(value);
	}
	void update() {
		clear();
		auto pe = current_database->end();
		for(auto pb = current_database->begin(); pb < pe; pb += current_database->getsize()) {
			if(current_list.allow && !current_list.allow(current_database, (int)current_database->indexof(pb)))
				continue;
			add(pb);
		}
	}
};

static void* getsourcep(void* source, int size, array* database) {
	if(size == sizeof(void*))
		return (void*)getsource(source, size);
	else
		return database->ptr(getsource(source, size));
}

static void setsourcep(void* source, int size, array* database, const void* value) {
	if(size == sizeof(void*))
		setsource(source, size, (long)value);
	else
		setsource(source, size, database->indexof(value));
}

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
	list.setvalue((void*)getsourcep(current_source, current_size, current_database));
	if(list.dropdown(rc))
		setsourcep(current_source, current_size, current_database, list.getvalue());
}

void draw::fielcd(const rect& rc, void* source, int size, array& database, const void* object, const fnlist& plist, bool instant) {
	hot.focus = rc;
	current_list = plist;
	current_object = object;
	current_database = &database;
	current_source = source;
	current_size = size;
	if(instant)
		post_combolist();
	else
		execute(post_combolist);
}

static void fielc(const rect& rc, unsigned flags, void* source, int size, array& database, const void* object, const fnlist& plist, const char* tips) {
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
	auto push_fore = fore;
	fore = colors::border;
	rectb(rc);
	fore = push_fore;
	rect rco = rc + metrics::edit;
	if(focused) {
		rect r1 = rco; r1.offset(-1);
		rectx(r1);
	}
	auto v = (void*)getsource(source, size);
	if(size != sizeof(void*))
		v = database.ptr((long)v);
	char temp[260]; stringbuilder sb(temp);
	auto pn = plist.getname(v, sb);
	if(pn)
		text(rco.x1, rco.y1, pn, -1, 0, rco.width());
	if(tips && a && !hot.pressed)
		tooltips(tips);
	auto execute_drop_down = false;
	if(a && hot.key == MouseLeft && !hot.pressed)
		execute_drop_down = true;
	if(focused && (hot.key == KeyEnter || hot.key==F2))
		execute_drop_down = true;
	if(execute_drop_down)
		fielcd(rc, source, size, database, object, plist, false);
}

void draw::field(const char* label, void* source, int size, int label_width, array& database, const fnlist& plist, const char* tips) {
	if(!plist.getname)
		return;
	auto push_caret = caret;
	auto push_width = width;
	setposition();
	titletext(label, label_width);
	rect rc = {caret.x, caret.y, caret.x + width, caret.y + draw::texth() + metrics::edit * 2};
	fielc(rc, AlignLeft | TextSingleLine, source, size, database, 0, plist, tips);
	width = push_width;
	caret = push_caret;
	caret.y += rc.height() + metrics::padding;
}