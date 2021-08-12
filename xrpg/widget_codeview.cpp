#include "draw.h"
#include "draw_control.h"
#include "draw_focus.h"
#include "draw_scroll.h"
#include "setting.h"

using namespace draw;
using namespace draw::controls;

//IllegalSymbol,
//WhiteSpace, Operator, Keyword, Comment,
//Number, String, Identifier,
//OpenParam, CloseParam, OpenBlock, CloseBlock, OpenScope, CloseScope,

static int common_padding = 4;
static bool use_space_instead_tab = false;

struct groupi {
	const char*		id;
	color			fore;
	unsigned		flags;
};

BSDATA(groupi) = {
	{"Illegal symbol", {255, 0, 0}},
	{"White space", {color(255, 255, 255)}},
	{"Operator", {color(255, 128, 0)}},
	{"Keyword", {color(0, 0, 128)}, TextBold},
	{"Comment", {color(0, 128, 0)}},
	{"Number", {color(128, 128, 0)}},
	{"String", {color(0, 255, 255)}},
	{"Identifier", {color(0, 0, 100)}},
	{"Open parameters block", {color(255, 0, 255)}},
	{"Close parameters block", {color(255, 0, 255)}},
	{"Open code block", {color(255, 0, 255)}},
	{"Close code block", {color(255, 0, 255)}},
	{"Open scope block", {color(255, 0, 255)}},
	{"Close scope block", {color(255, 0, 255)}},
	{"Open user block", {color(0, 50, 100)}},
	{"Close user block", {color(0, 50, 128)}},
};

static const sprite* default_font = (sprite*)loadb("art/fonts/code.pma");
static point fontsize;

class widget_codeview : public control, vector<char> {

	point pos1, pos2, size;
	int	maximum_x, maximum_y, origin_x, origin_y;
	int cash_origin, p1, p2;
	bool readonly;

	int getlinesperpage() const {
		return client.height() / fontsize.y;
	}

	void invalidate() {
		cash_origin = -1;
	}

	void ensurevisible(int cw) {
		if(cw < 0 || cw >= maximum_y)
			return;
		if(cw < origin_y) {
			origin_y = cw;
			invalidate();
		}
		auto lines_per_page = getlinesperpage();
		if(cw > origin_y + (lines_per_page - 1)) {
			origin_y = cw - (lines_per_page - 1);
			invalidate();
		}
	}

	int	hittest(rect rc, point pt, unsigned state) const {
		return -1;
	}

	void left(bool shift, bool ctrl) {
		//const char* p = ptr(p1);
		//if(!ctrl)
		//	p = nextstep(p, -1);
		//else {
		//	while(p > data && iswhitespace(p[-1]))
		//		p = nextstep(p, -1);
		//	if(p > data) {
		//		auto result = isidentifier(p[-1]);
		//		while(p > data && isidentifier(p[-1]) == result)
		//			p = nextstep(p, -1);
		//	}
		//}
		//set(p - begin(), shift);
	}

	void right(bool shift, bool ctrl) {
		//const char* p = ptr(p1);
		//if(!ctrl)
		//	p = nextstep(p, 1);
		//else {
		//	auto pe = end();
		//	while(p < pe && iswhitespace(*p))
		//		p = nextstep(p, 1);
		//	if(p < pe) {
		//		auto result = isidentifier(*p);
		//		while(p > data && isidentifier(*p) == result)
		//			p = nextstep(p, 1);
		//	}
		//}
		//set(p - begin(), shift);
	}

	static void execute_wordselect() {
		//auto p = (widget_codeview*)hot.object;
		//p->left(false, true);
		//p->right(true, true);
	}

	int	getbegin() const {
		if(p2 == -1)
			return p1;
		return imin(p1, p2);
	}

	int	getend() const {
		if(p2 == -1)
			return p1;
		return imax(p1, p2);
	}

	void clear() {
		if(p2 != -1 && p1 != p2 && data) {
			auto s1 = ptr(getbegin());
			auto s2 = ptr(getend());
			while(*s2)
				*s1++ = *s2++;
			*s1 = 0;
			invalidate();
			if(p1 > p2)
				p1 = p2;
		}
		p2 = -1;
	}

	const char* nextstep(const char* ps, int dir) {
		if(*ps == 10) {
			ps += dir;
			if(*ps == 13)
				ps += dir;
		} else if(*ps == 13) {
			ps += dir;
			if(*ps == 10)
				ps += dir;
		} else
			ps += dir;
		if(ps < begin())
			return begin();
		else if(ps >= (end() - 1))
			return end() - 1;
		return ps;
	}

	int getlenght() const {
		return getcount();
	}

	void correction() {
		auto lenght = getlenght();
		if(p2 != -1 && p2 > lenght) {
			p2 = lenght;
			invalidate();
		}
		if(p1 > lenght) {
			p1 = lenght;
			invalidate();
		}
		if(p1 < 0) {
			p1 = 0;
			invalidate();
		}
	}

	void redraw(const rect& rco) const {
		if(!fontsize.x || !fontsize.y)
			return;
		auto push_font = font;
		draw::font = default_font;
		rect rc = rco; rc.offset(4, 4);
		// Mouse input handle
		rect r1 = rc;
		rc.y1 -= origin_y * fontsize.y;
		rc.x1 -= origin_x;
		auto x = rc.x1, y = rc.y1;
		if(ishilite(r1)) {
			point mpos;
			mpos.x = (hot.mouse.x - r1.x1 + origin_x + fontsize.x / 2) / fontsize.x;
			mpos.y = (hot.mouse.y - r1.y1) / fontsize.y + origin_y;
			auto x1 = rc.x1 + mpos.x * fontsize.x;
			auto y1 = rc.y1 + mpos.y * fontsize.y;
			rect r2 = {x1, y1, x1 + fontsize.x, y1 + fontsize.y};
			ishilite(r2);
			switch(hot.key) {
			case MouseLeft:
			case MouseLeft | Shift:
				if(hot.pressed) {
					//auto i = getindex(mpos);
					//if(hot.key & Shift)
					//	post("select_range", i);
					//else
					//	post("select", i);
				}
				break;
			case MouseMove:
				if(hot.pressed) {
					//auto i = getindex(mpos);
					//post("select_range", i);
				}
				break;
			case MouseLeftDBL:
				if(hot.pressed)
					draw::execute(execute_wordselect, 0, 0, this);
				break;
			}
		}
		point pos = {};
		auto ps = begin();
		//while(true) {
		//	auto x1 = x + pos.x * fontsize.x;
		//	auto y1 = y + pos.y * fontsize.y;
		//	auto c = getnext(ps, pos, type);
		//	if(!c)
		//		break;
		//	auto& ei = bsdata<groupi>::elements[type].visual;
		//	fore = ei.present;
		//	text(x1, y1, ps, c, ei.flags);
		//	ps += c;
		//}
		// Draw hilite
		if(true) {
			auto x1 = x + pos1.x * fontsize.x;
			auto y1 = y + pos1.y * fontsize.y;
			line(x1, y1, x1, y1 + fontsize.y, colors::text.mix(colors::active));
		}
		if(getbegin() != -1) {
			//auto p1 = getbeginpos();
			//auto p2 = getendpos();
			//for(auto i = p1.y; i <= p2.y; i++) {
			//	auto x1 = rco.x1;
			//	auto x2 = rco.x2;
			//	auto y1 = y + i * fontsize.y;
			//	if(i == p1.y)
			//		x1 = x + p1.x * fontsize.x;
			//	if(i == p2.y)
			//		x2 = x + p2.x * fontsize.x;
			//	rectf({x1, y1, x2, y1 + fontsize.y}, colors::active, 64);
			//}
		}
		font = push_font;
	}

	void paint(const rect& rc) override {
		if(cash_origin == -1) {
			//getstate(p1, pos1, p2, pos2, size, origin_y, cash_origin);
			maximum_x = size.x * fontsize.x;
			maximum_y = size.y;
		}
		auto pixels_per_line = fontsize.y;
		if(!pixels_per_line)
			return;
		auto focused = draw::isfocused(this);
		rect rce = rc; rce.offset(4, 4);
		control::paint(rce);
		auto lines_per_page = client.height() / pixels_per_line;
		scroll scrollv(origin_y, lines_per_page, maximum_y, client); scrollv.input();
		scroll scrollh(origin_x, client.width(), maximum_x, client, true); scrollh.input();
		auto push_clip = clipping; setclip(client);
		redraw(client);
		clipping = push_clip;
		scrollv.view(focused);
		scrollh.view(focused);
	}

public:

	void setvalue(const char* id, long value) override {
		if(equal(id, "readonly"))
			readonly = (value != 0);
	}

};

class widget_codeview_plugin : controls::control::plugin, controls::control::plugin::builder {
	builder* getbuilder() override {
		return this;
	}
	bool canopen(const char* url) const override {
		auto ext = szext(url);
		if(equal(ext, "cpp"))
			return true;
		return false;
	}
	control* create(const char* url) override {
		return new widget_codeview();
	}
public:
	widget_codeview_plugin() : plugin("Codeview", dock::Workspace) {}
};

static setting::element format_setting[] = {{"Tabulation", {common_padding}},
	{"»спользовать пробелы вместо табул€ции", {use_space_instead_tab}},
};
static setting::header headers[] = {{"Editor", "General", "Formating", format_setting},
};

void initialize_codeview() {
	auto old_font = draw::font;
	draw::font = default_font;
	fontsize.x = draw::textw('A');
	fontsize.y = draw::texth();
	draw::font = old_font;
}