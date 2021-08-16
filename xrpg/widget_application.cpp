#include "crt.h"
#include "draw.h"
#include "draw_button.h"
#include "draw_control.h"
#include "draw_dockbar.h"
#include "draw_focus.h"
#include "draw_scroll.h"
#include "handler.h"
#include "io_plugin.h"
#include "setting.h"

using namespace	setting;
using namespace	draw;
using namespace	draw::controls;

struct application_window {
	int						x, y, width, height;
	int						header_width;
	unsigned				flags;
};

typedef adat<control*>		controla;
bool						metrics::show::padding;
bool						metrics::show::statusbar;
bool						metrics::show::bottom;
bool						metrics::show::top;
bool						metrics::show::left;
bool						metrics::show::right;
static bool					use_short_name_label;
static bool					use_no_extension_label;
static bool					use_uppercase_label;
static const header*		current_header;
static int					current_tab;
static char					last_open_file[260];
static controls::control*	current_active_control;
static application_window	window = {0, 0, 0, 0, 160, WFMinmax | WFResize};
static const char*			settings_file_name = "settings.json";
static vector<controls::control*> active_controls;

void getdocked(controla& result, dock type);

struct streca {
	const element*	e;
	const header*	h;
};

static int getpriority(const header* p) {
	if(strcmp(p->page, "General") == 0)
		return 1;
	return 10;
}

static int compare(const char* s1, const char* s2) {
	if(s1 == s2)
		return 0;
	if(!s1)
		return -1;
	if(!s2)
		return 1;
	return strcmp(getnm(s1), getnm(s2));
}

static int compare(const header* h1, const header* h2) {
	auto c1 = compare(h1->division, h2->division);
	if(c1)
		return c1;
	auto r1 = getpriority(h1);
	auto r2 = getpriority(h2);
	if(r1 != r2)
		return r1 - r2;
	auto c2 = compare(h1->page, h2->page);
	if(c2)
		return c2;
	return compare(h1->group, h2->group);
}

class stelemsa : public adat<streca, 128> {
	static int compare_elements(const void* p1, const void* p2) {
		auto e1 = (streca*)p1;
		auto e2 = (streca*)p2;
		auto c1 = compare(e1->h, e2->h);
		if(c1)
			return c1;
		return strcmp(e1->e->name, e2->e->name);
	}
public:
	void select() {
		for(auto pm = header::first; pm; pm = pm->next) {
			for(auto& e : pm->elements) {
				auto p = add();
				p->e = &e;
				p->h = pm;
			}
		}
	}
	void match_filled() {
		auto ps = data;
		for(auto& e : *this) {
			if(e.e->var.iszero())
				continue;
			*ps++ = e;
		}
		count = ps - data;
	}
	void match_visible() {
		auto ps = data;
		for(auto& e : *this) {
			if(e.e->test && !e.e->test())
				continue;
			if(e.h->visible && !e.h->visible())
				continue;
			*ps++ = e;
		}
		count = ps - data;
	}
	void sort() {
		qsort(data, count, sizeof(data[0]), compare_elements);
	}
};

class strowsa : public adat<const header*, 128> {
	static int compare_elements(const void* p1, const void* p2) {
		return compare(*((header**)p1), *((header**)p2));
	}
	bool hasdivision(const header* p) const {
		for(auto pe : *this) {
			if(strcmp(p->division, pe->division) == 0)
				return true;
		}
		return false;
	}
	bool haspage(const header* p) const {
		for(auto pe : *this) {
			if(strcmp(p->page, pe->page) == 0)
				return true;
		}
		return false;
	}
public:
	void sort() {
		if(count)
			qsort(data, count, sizeof(data[0]), compare_elements);
	}
	void select_divisions() {
		for(auto pm = header::first; pm; pm = pm->next) {
			if(hasdivision(pm))
				continue;
			add(pm);
		}
	}
	void select_pages(const header* parent) {
		for(auto pm = header::first; pm; pm = pm->next) {
			if(strcmp(parent->division, pm->division) != 0)
				continue;
			if(haspage(pm))
				continue;
			add(pm);
		}
	}
	void select_groups(const header* parent) {
		for(auto pm = header::first; pm; pm = pm->next) {
			if(strcmp(parent->division, pm->division) != 0)
				continue;
			if(strcmp(parent->page, pm->page) != 0)
				continue;
			add(pm);
		}
	}
	void select_divisions(const stelemsa& source) {
		for(auto& e : source) {
			if(hasdivision(e.h))
				continue;
			add(e.h);
		}
	}
	void select_pages(const stelemsa& source, const header* parent) {
		for(auto& e : source) {
			if(strcmp(parent->division, e.h->division) != 0)
				continue;
			if(haspage(e.h))
				continue;
			add(e.h);
		}
	}
	void select_groups(const stelemsa& source, const header* parent) {
		for(auto& e : source) {
			if(strcmp(parent->division, e.h->division) != 0)
				continue;
			if(strcmp(parent->page, e.h->page) != 0)
				continue;
			if(haspage(e.h))
				continue;
			add(e.h);
		}
	}
};

static const char* get_page_name(const void* p, stringbuilder& sb) {
	return getnm(((header*)p)->page);
}

static void callback_settab() {
	current_tab = hot.param;
}

static void callback_button() {
	auto p = (element*)hot.param;
	if(p->var.type == Button && p->var.data)
		((pcall)p->var.data)();
}

static bool choose_folder(const void* object, array& source, void* pointer) {
	auto p = (const char**)pointer;
	char temp[260]; memset(temp, 0, sizeof(temp));
	if(*p)
		zcpy(temp, *p, sizeof(temp) - 1);
	if(!draw::dialog::folder(getnm("ChooseFolder"), temp))
		return false;
	*p = szdup(temp);
	return true;
}

static void callback_choose_color() {
	auto p = (color*)hot.param;
	draw::dialog::choose(*p);
}

static const char* getname(char* temp, const setting::element& e) {
	zcpy(temp, getnm(e.name));
	temp[0] = szupper(temp[0]);
	return temp;
}

static void button(int x, int& y, int width, const char* title, const element& e) {
	setposition(x, y, width, 1);
	rect rc = {x, y, x + width, y + texth() + 8};
	auto result = false;
	auto focus = isfocused(rc, &e);
	if(button(rc, title, 0, 0, colors::button, focus, false, false, true))
		draw::execute(callback_button, (size_t)&e);
	y += rc.height() + metrics::padding;
}

static int render_element(int x, int y, int width, unsigned flags, const setting::element& e) {
	const auto title = 200;
	char temp[512]; temp[0] = 0;
	if(e.test && !e.test())
		return 0;
	auto y1 = y; int d;
	switch(e.var.type) {
	case setting::Radio:
		radio(x, y, width, e.var.data, sizeof(int), e.var.value, getname(temp, e), 0);
		break;
	case setting::Bool:
		checkbox(x, y, width, e.var.data, sizeof(bool), 1, getname(temp, e), 0);
		break;
	case setting::Number:
		d = 0;
		if(e.param) {
			d = e.param;
			auto w = (d + 1) * textw("0") + metrics::padding * 2 + 19 + 4;
			if(title + w < width)
				width = title + w;
		}
		fieln(x, y, width, getnm(e.name), e.var.data, e.var.size, title, d);
		break;
	case setting::Color:
		y += field(x, y, width, getnm(e.name), *((color*)e.var.data), title, 0);
		break;
	case setting::Button:
		button(x, y, width, getnm(e.name), e);
		break;
	case setting::Text:
		field(x, y, width, getnm(e.name), *((const char**)e.var.data), title, 0);
		break;
	case setting::Url:
		field(x, y, width, getnm(e.name), *((const char**)e.var.data), title, choose_folder);
		break;
	case setting::Control:
		break;
	}
	return y - y1;
}

static int render_element(int x, int y, int width, unsigned flags, const setting::header& e) {
	auto x1 = x, y2 = y, w1 = width;
	auto height = draw::texth() + metrics::padding * 2;
	if(e.group) {
		y += height;
		x1 += metrics::padding;
		w1 -= metrics::padding * 2;
	}
	auto yc = y;
	for(auto& pe : e.elements)
		y += render_element(x1, y, w1, flags, pe);
	if(y == yc)
		return 0;
	if(e.group) {
		color c1 = colors::border.mix(colors::window, 128);
		color c2 = c1.darken();
		gradv({x, y2, x + width, y2 + height}, c1, c2);
		auto push_fore = fore;
		fore = colors::text.mix(c1, 196);
		auto label = getnm(e.group);
		text(x + (width - textw(label)) / 2, y2 + metrics::padding, label);
		fore = push_fore;
		rectb({x, y2, x + width, y + metrics::padding}, colors::border);
		y += metrics::padding * 2;
	}
	return y - y2;
}

static struct widget_settings_header : controls::list {
	strowsa rows;
	void initialize() {
		rows.clear();
		rows.select_divisions();
		rows.sort();
		if(current >= getmaximum() - 1)
			current = getmaximum();
		if(current < 0)
			current = 0;
	}
	void row(const rect& rc, int index) const override {
		list::row({rc.x1 + 1, rc.y1 + 1, rc.x2 - 1, rc.y2}, index);
		textc(rc.x1 + metrics::edit, rc.y1 + metrics::edit, rc.width() - metrics::edit * 2, getnm(rows[index]->division));
	}
	const header* getcurrent() {
		return rows[current];
	}
	int getmaximum() const override {
		return rows.getcount();
	}
} setting_header;

static struct widget_control_viewer : controls::tableref {
	static const char* get_id_name(const void* object, stringbuilder& sb) {
		return getnm(((plugin*)object)->id);
	}
	void initialize() {
		no_change_order = true;
		no_change_count = true;
		addcol("Name", ANREQ(plugin, id), "Text").set(columnf::ReadOnly).plist.getname = get_id_name;
		addcol("Docking", ANREQ(plugin, position), "Enum", bsdata<docki>::source_ptr);
		//addcol(type, "visible", "Видимость", "checkbox");
		for(auto p = plugin::first; p; p = p->next) {
			auto pc = p->getcontrol();
			if(!pc)
				continue;
			addref(p);
		}
	}
	bool hastoolbar() const override {
		return false;
	}
} control_viewer;

int draw::field(int x, int& y, int width, const char* label, color& value, int header_width, const char* tips) {
	char temp[128]; stringbuilder sb(temp);
	sb.add("%1i, %2i, %3i", value.r, value.g, value.b);
	setposition(x, y, width);
	titletext(x, y, width, label, header_width);
	auto push_fore = fore;
	if(value.isdark())
		fore = colors::white;
	else
		fore = colors::black;
	rect rc = {x, y, x + width, y + draw::texth() + 8};
	auto focused = isfocused(rc, &value);
	if(button(rc, temp, tips, 0, value, focused, false, false, true))
		execute(callback_choose_color, (size_t)&value);
	fore = push_fore;
	return rc.height() + metrics::padding * 2;
}

const char* draw::getlabel(const void* object, stringbuilder& sb) {
	auto pc = (controls::control*)object;
	auto p = pc->getvalue("Name", sb);
	if(p != sb.begin())
		sb.add(p);
	char temp[260]; stringbuilder sb1(temp);
	p = pc->getvalue("URL", sb1);
	if(p) {
		if(use_short_name_label)
			sb.add(szfname(p));
		else
			sb.add(p);
		if(use_no_extension_label) {
			char* p1 = (char*)szext(sb);
			if(p1) {
				while(p1 > sb.begin() && p1[-1] == '.')
					p1--;
				sb.set(p1);
			}
		}
	}
	if(pc->ismodified())
		sb.add("*");
	if(use_uppercase_label)
		sb.begin()[0] = szupper(sb.begin()[0]);
	return sb;
}

static struct widget_settings : controls::control {
	const char* getvalue(const char* id, stringbuilder& sb) const override {
		if(equal(id, "Name"))
			return getnm("Settings");
		return 0;
	}
	void paint(const rect& rcorigin) override {
		auto rc = rcorigin;
		auto push_fore = fore;
		fore = colors::text;
		splitv(rc.x1, rc.y1, window.header_width, rc.height(), 6, 64, 282, false);
		setting_header.view({rc.x1, rc.y1, rc.x1 + window.header_width, rc.y2}, metrics::show::padding, true);
		rc.x1 += window.header_width + 6;
		auto top = setting_header.getcurrent();
		if(top != current_header) {
			current_header = top;
			current_tab = -1;
		}
		if(!top)
			return;
		strowsa tabs; tabs.select_pages(top);
		if(!tabs)
			return;
		tabs.sort();
		if(current_tab == -1)
			current_tab = 0;
		auto h1 = 28;
		// Paint tabs
		auto hilited = -1;
		if(metrics::show::padding)
			rectb({rc.x1, rc.y1 + h1, rc.x2, rc.y2}, colors::border);
		else
			line(rc.x1, rc.y1 + h1, rc.x2 - metrics::padding, rc.y1 + h1, colors::border);
		if(draw::tabs({rc.x1, rc.y1, rc.x2, rc.y1 + h1}, false, false,
			(void**)tabs.data, 0, tabs.count, current_tab, &hilited,
			get_page_name, 0, colors::form)) {
			draw::execute(callback_settab);
			hot.param = hilited;
		}
		if(metrics::show::padding)
			rc.offset(metrics::padding + 2, metrics::padding);
		else
			rc.y1 += metrics::padding;
		// Paint current tab
		if(current_tab == -1)
			return;
		auto p1 = tabs[current_tab];
		strowsa elements; elements.select_groups(p1);
		if(!elements)
			return;
		elements.sort();
		int w3, w4;
		auto e1 = p1->elements.begin();
		switch(e1->var.type) {
		case setting::Control:
			rc.y1 += h1;
			if(metrics::show::padding)
				rc.y1 += metrics::padding;
			((control*)e1->var.data)->view(rc, metrics::show::padding, true);
			break;
		default:
			rc.y1 += h1 + metrics::padding;
			w4 = rc.width();
			w3 = imin(w4, 620);
			for(auto pe : elements)
				rc.y1 += render_element(rc.x1, rc.y1, w3, 0, *pe);
			break;
		}
	}
} widget_settings_control;

static void post_activate() {
	activate((control*)hot.object);
}

static void post_close() {
	close((control*)hot.object);
}

static control* getactivated() {
	return current_active_control;
}

void draw::activate(control* p) {
	if(current_active_control)
		current_active_control->deactivating();
	current_active_control = p;
	if(current_active_control) {
		if(current_active_control->isfocusable())
			setfocus(current_active_control, 0, true);
		current_active_control->activating();
	}
}

void draw::close(control* p) {
	auto i = active_controls.indexof(p);
	if(i == -1)
		return;
	if(current_active_control == p) {
		if(i > 0)
			activate(active_controls[i - 1]);
		else if(i < (int)(active_controls.getcount() - 1))
			activate(active_controls[i + 1]);
		else
			p->deactivating();
	}
	active_controls.remove(i, 1);
	delete p;
}

static struct widget_application : draw::controls::control {
	fnevent	heartproc;
	const char* getvalue(const char* id, stringbuilder& sb) const override {
		if(equal(id, "Name"))
			return getnm("Main");
		return 0;
	}
	bool isfocusable() const override {
		return false;
	}
	static void getactivepages(controla& result) {
		for(auto p : active_controls)
			result.add(p);
	}
	static void show_statusbar(const control* pc) {
		char temp[260]; stringbuilder sb(temp);
		auto pu = pc->getvalue("URL", sb);
		if(pu)
			statusbar(getnm("DataFormURL"), pu);
	}
	static int getindexof(const controla& source, control* v) {
		auto i = 0;
		for(auto e : source) {
			if(e == v)
				return i;
			i++;
		}
		return -1;
	}
	static void workspace(rect rc) {
		controla ct;
		getdocked(ct, dock::Workspace); auto z1 = ct.getcount();
		getactivepages(ct);
		if(ct.getcount() == 0) {
			auto push_fore = fore;
			fore = colors::border;
			text(rc, getnm("NotFoundOpenDocuments"), AlignCenterCenter);
			fore = push_fore;
		} else if(ct.getcount() == 1) {
			current_active_control = ct[0];
			current_active_control->view(rc, metrics::show::padding, false);
		} else {
			auto current_select = getindexof(ct, current_active_control);
			if(current_select == -1)
				current_select = 0;
			const int dy = draw::texth() + 8;
			rect rct = {rc.x1, rc.y1, rc.x2, rc.y1 + dy};
			rc.y1 += dy;
			if(current_active_control)
				current_active_control->view(rc, metrics::show::padding, true, false);
			line(rct.x1, rct.y2, rct.x2, rct.y2, colors::border);
			auto current_hilite = -1;
			auto result = draw::tabs(rct, false, false, (void**)ct.begin(), 0, z1,
				current_select, &current_hilite, getlabel, &rct.x1, colors::window);
			if(ct.getcount() != 0) {
				auto current_hilite2 = -1;
				auto r1 = draw::tabs(rct, true, false, (void**)ct.begin(), z1, ct.getcount(),
					current_select, &current_hilite2, getlabel, &rct.x1, colors::window);
				if(current_hilite2 != -1)
					current_hilite = current_hilite2;
				if(r1)
					result = r1;
			}
			if(current_hilite != -1) {
				show_statusbar(ct.begin()[current_hilite]);
				switch(result) {
				case 1: draw::execute(post_activate, 0, 0, ct.begin()[current_hilite]); break;
				case 2: draw::execute(post_close, 0, 0, ct.begin()[current_hilite]); break;
				}
			}
		}
	}
	void paint(const rect& rc) override {
		auto rct = rc;
		if(heartproc)
			heartproc();
		dockbar(rct);
		workspace(rct);
	}
	static const char* commands_file[];
	static const char* commands[];
	const char** getcommands() const override {
		return commands_file;
	}
	const char** getcommands(const char* id) const override {
		if(equal(id, "File"))
			return commands_file;
		return 0;
	}
	void create_filter(stringbuilder& sb) {
		io::plugin::addfilter(sb, getnm("AllFiles"), "*.*");
		for(auto p = plugin::first; p; p = p->next) {
			auto pc = p->getbuilder();
			if(!pc)
				continue;
			pc->getextensions(sb);
		}
		sb.addsz();
	}
	bool execute(const char* id, bool run) override {
		if(equal(id, "Open")) {
			if(run) {
				static char filter[1024 * 16];
				stringbuilder sb(filter); create_filter(sb);
				if(!dialog::open(getnm("OpenFile"), last_open_file, filter, -1))
					return false;
				openurl(last_open_file);
			}
		} else if(equal(id, "Save")) {
			auto p = getactivated();
			if(!p)
				return false;
			if(!p->ismodified())
				return false;
			if(run) {
				char temp[512]; stringbuilder sb(temp);
				auto ps = p->getvalue("URL", sb);
				if(!ps || ps[0] == 0) {
					ps = temp; temp[0] = 0;
					if(!dialog::save(getnm("SaveFile"), temp, 0))
						return false;
					p->setvalue("URL", (int)szdup(temp));
				}
				p->execute("Save", true);
			}
		} else
			return control::execute(id, run);
		return true;
	}
} widget_application_control;

const char* widget_application::commands_file[] = {"Create", "Open", "Save", 0};
const char* widget_application::commands[] = {"File", 0};

control* draw::openurl(const char* url) {
	char temp[260];
	for(auto p : active_controls) {
		stringbuilder sb(temp);
		auto purl = p->getvalue("URL", sb);
		if(!purl)
			continue;
		if(szcmpi(purl, url) == 0) {
			activate(p);
			return p;
		}
	}
	for(auto p = plugin::first; p; p = p->next) {
		auto pc = p->getbuilder();
		if(!pc)
			continue;
		if(!pc->canopen(url))
			continue;
		if(pc->read(url))
			return 0;
		auto result = pc->create();
		if(result) {
			result->setvalue("URL", (int)url);
			result->execute("Open", true);
			active_controls.add(result);
			activate(result);
			return result;
		}
	}
	return 0;
}

void set_light_theme();
void set_dark_theme();

static const element appearance_general_metrics[] = {
	{"Padding", metrics::padding, 3},
	{"ScrollWidth", metrics::scroll, 3},
};
static const element colors_general[] = {
	{"SetLightTheme", set_light_theme},
	{"SetDarkTheme", set_dark_theme},
};
static const element colors_form[] = {
	{"TextColor", colors::text},
	{"WindowColor", colors::window},
	{"FormColor", colors::form},
	{"BorderColor", colors::border},
	{"ActiveColor", colors::active},
	{"ButtonColor", colors::button},
	{"TipsBackColor", colors::tips::back},
	{"TipsTextColor", colors::tips::text},
};
static const element appearance_general_view[] = {
	{"ShowStatusBar", metrics::show::statusbar},
	{"ShowLeftPanel", metrics::show::left},
	{"ShowRightPanel", metrics::show::right},
	{"ShowBottomPanel", metrics::show::bottom},
	{"ShowPadding", metrics::show::padding},
};
static const element appearance_general_tabs[] = {
	{"UseShortNameLabel", use_short_name_label},
	{"NoUrlLabelExtension", use_no_extension_label},
	{"UppercaseLabel", use_uppercase_label},
};
static const element plugin_elements[] = {{0, {Control, static_cast<control*>(&control_viewer), 0}}};
static header setting_headers[] = {
	{"Workspace", "General", "Metrics", appearance_general_metrics},
	{"Workspace", "General", "View", appearance_general_view},
	{"Workspace", "General", "Tabs", appearance_general_tabs},
	{"Colors", "General", 0, colors_general},
	{"Colors", "Form", 0, colors_form},
	{"Workspace", "Plugins", 0, plugin_elements},
};
static controls::control* layouts[] = {&widget_application_control, &widget_settings_control};

static void get_control_status(controls::control* object) {
	char temp[260]; stringbuilder sb(temp);
	draw::statusbar("Переключить вид на '%1'", object->getvalue("Name", sb));
}

bool draw::edit(control& e, fnevent heartbeat) {
	while(ismodal()) {
		auto tb = e.getimages();
		rect rc = {0, 0, draw::getwidth(), draw::getheight()};
		draw::rectf(rc, colors::form);
		if(metrics::show::statusbar)
			draw::statusbar(rc);
		rect rt = rc;
		if(tb)
			rt.y2 = rt.y1 + tb->get(0).getrect(0, 0, 0).height() + 4 * 2;
		else
			rt.y2 = rt.y1 + 24 + 4 * 2;
		sheetline(rt, true);
		rc.y1 += rt.height();
		rt.x1 += 2; rt.y1 += 1; rt.x2 -= 2;
		auto dx = draw::texth() + metrics::padding + 4 * 2;
		auto y2 = rc.y2 - dx; rc.y2 = y2;
		if(metrics::show::padding) {
			rc.x1 += metrics::padding;
			rc.x2 -= metrics::padding;
			rc.y1 += metrics::padding;
		}
		auto x2 = rc.x2;
		e.view(rc, true, true);
		buttonr(x2, y2, getnm("Cancel"), buttoncancel, KeyEscape);
		buttonr(x2, y2, getnm("OK"), buttonok, KeyEnter);
		domodal();
		if(heartbeat)
			heartbeat();
	}
	return getresult() != 0;
}

stringbuilder& getstatustext();

void draw::application() {
	auto current_tab = 0;
	while(ismodal()) {
		if(current_tab >= sizeof(layouts) / sizeof(layouts[0]))
			current_tab = sizeof(layouts) / sizeof(layouts[0]) - 1;
		auto pc = layouts[current_tab];
		auto tb = pc->getimages();
		draw::fore = colors::text;
		rect rc = {0, 0, draw::getwidth(), draw::getheight()};
		draw::rectf(rc, colors::form);
		if(metrics::show::statusbar)
			statusbar(rc);
		rect rt = rc;
		if(tb)
			rt.y2 = rt.y1 + tb->get(0).getrect(0, 0, 0).height() + 4 * 2;
		else
			rt.y2 = rt.y1 + 24 + 4 * 2;
		sheetline(rt, true);
		rc.y1 += rt.height();
		rt.offset(2, 1);
		if(metrics::show::padding)
			rc.offset(metrics::padding);
		pc->paint(rc);
		pc->toolbar(rt.x1, rt.y1, rt.width(), &rt.x1);
		if(current_active_control && pc==&widget_application_control)
			current_active_control->toolbar(rt.x1, rt.y1, rt.width());
		auto hilite_tab = -1;
		auto reaction = draw::tabs(rt, false, true, (void**)layouts, 0,
			sizeof(layouts) / sizeof(layouts[0]), current_tab, &hilite_tab,
			getlabel, 0, colors::form);
		if(hilite_tab != -1)
			get_control_status(layouts[hilite_tab]);
		if(getstatustext().isempthy())
			statusbar(getnm("Ready"));
		domodal();
		if(reaction == 1)
			current_tab = hilite_tab;
		switch(hot.key) {
		case F2: metrics::show::bottom = !metrics::show::bottom; break;
		case Alt + F2: metrics::show::left = !metrics::show::left; break;
		case Ctrl + F2: metrics::show::right = !metrics::show::right; break;
		default: break;
		}
	}
}

void set_dark_theme();
void set_light_theme();

static int getnum(const char* value) {
	if(strcmp(value, "true") == 0)
		return 1;
	if(strcmp(value, "false") == 0)
		return 0;
	if(strcmp(value, "null") == 0)
		return 0;
	int int_value = 0;
	stringbuilder::read(value, int_value);
	return int_value;
}

static struct settings_settings_strategy : io::strategy {
	void write(serializer& file, const element& e) {
		switch(e.var.type) {
		case setting::Bool:
		case setting::Number:
		case setting::Color:
			file.set(e.name, e.var.get());
			break;
		case setting::Radio:
			file.set(e.name, 1);
			break;
		case setting::Text:
		case setting::Url:
			file.set(e.name, (const char*)e.var.get());
			break;
		default:
			break;
		}
	}
	void write(serializer& file, void* param) override {
		strowsa divisions; divisions.select_divisions();
		for(auto pd : divisions) {
			strowsa pages; pages.select_pages(pd);
			if(!pages)
				continue;
			file.open(pd->division);
			for(auto pg : pages) {
				strowsa groups; groups.select_groups(pg);
				if(!groups)
					continue;
				file.open(pg->page);
				for(auto gr : groups) {
					if(gr->group)
						file.open(gr->group);
					for(auto& e : gr->elements)
						write(file, e);
					if(gr->group)
						file.close(gr->group);
				}
				file.close(pg->page);
			}
			file.close(pd->division);
		}
	}
	static const element* find(serializer::node& n) {
		auto name = n.name;
		if(!name)
			return 0;
		if(!n.parent || !n.parent->parent || !n.parent->parent->parent)
			return 0;
		const char* division = 0;
		const char* page = 0;
		const char* group = 0;
		if(strcmp(n.parent->parent->parent->name, "settings") == 0) {
			division = n.parent->parent->name;
			page = n.parent->name;
		} else {
			if(!n.parent->parent->parent->parent)
				return 0;
			if(strcmp(n.parent->parent->parent->parent->name, "settings") != 0)
				return 0;
			division = n.parent->parent->parent->name;
			page = n.parent->parent->name;
			group = n.parent->name;
		}
		if(!division || !page)
			return 0;
		for(auto pm = header::first; pm; pm = pm->next) {
			if(strcmp(pm->division, division) != 0)
				continue;
			if(strcmp(pm->page, page) != 0)
				continue;
			if(pm->group) {
				if(!group || strcmp(pm->group, group) != 0)
					continue;
			} else {
				if(group)
					continue;
			}
			for(auto& e : pm->elements) {
				if(strcmp(e.name, name) == 0)
					return &e;
			}
		}
		return 0;
	}
	void set(serializer::node& n, const char* value) override {
		auto e = find(n);
		if(!e)
			return;
		switch(e->var.type) {
		case setting::Number:
		case setting::Color:
		case setting::Bool:
			e->var.set(getnum(value));
			break;
		case setting::Radio:
			e->var.set(e->var.value);
			break;
		case setting::Text:
		case setting::Url:
			e->var.set((size_t)szdup(value));
			break;
		default:
			break;
		}
	}
	settings_settings_strategy() : strategy("settings", "settings") {}
} settings_settings_strategy_instance;

static struct window_settings_strategy : io::strategy {
	void write(serializer& file, void* param) override {
		file.set("x", window.x);
		file.set("y", window.y);
		file.set("width", window.width);
		file.set("height", window.height);
		file.set("header_width", window.header_width);
		file.set("flags", window.flags);
	}
	void set(serializer::node& n, const char* value) override {
		if(n == "x")
			window.x = getnum(value);
		else if(n == "y")
			window.y = getnum(value);
		else if(n == "width")
			window.width = getnum(value);
		else if(n == "height")
			window.height = getnum(value);
		else if(n == "header_width")
			window.header_width = getnum(value);
		else if(n == "flags")
			window.flags = getnum(value);
	}
	window_settings_strategy() : strategy("window", "settings") {}
} window_settings_strategy_instance;

static struct controls_settings_strategy : io::strategy {
	void write(serializer& file, void* param) override {
		for(auto pp = controls::plugin::first; pp; pp = pp->next) {
			auto id = pp->id;
			if(!id || id[0] == 0)
				continue;
			file.open(id);
			file.set("Docking", static_cast<int>(pp->position));
			file.set("Visible", pp->visible);
			auto pc = pp->getcontrol();
			if(!pc)
				continue;
			pc->write(file);
			file.close(id);
		}
	}
	void set(serializer::node& n, const char* value) override {
		if(!n.parent || !n.parent->parent)
			return;
		auto e = const_cast<controls::plugin*>(controls::plugin::find(n.parent->name));
		if(!e)
			return;
		else if(n == "Docking") {
			int int_value = 0;
			stringbuilder::read(value, int_value);
			e->position = static_cast<dock>(int_value);
		} else if(n == "Visible")
			e->visible = getnum(value);
	}
	controls_settings_strategy() : strategy("controls", "settings") {}
} controls_settings_strategy_instance;

static void exit_application() {
	point pos, size; getwindowpos(pos, size, &window.flags);
	if((window.flags & WFMaximized) == 0) {
		window.x = pos.x;
		window.y = pos.y;
		window.width = size.x;
		window.height = size.y;
	}
	io::write(settings_file_name, "settings", 0);
}

void draw::initialize(const char* title) {
	set_light_theme();
	atexit(exit_application);
	io::read(settings_file_name, "settings", 0);
	after_initialize->execute();
	setting_header.initialize();
	control_viewer.initialize();
	draw::font = metrics::font;
	draw::fore = colors::text;
	draw::fore_stroke = colors::blue;
	create(window.x, window.y, window.width, window.height, window.flags, 32);
	setcaption(title);
	setnext(application);
}