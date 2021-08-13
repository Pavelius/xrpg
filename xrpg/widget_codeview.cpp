#include "codescan.h"
#include "draw.h"
#include "draw_clipboard.h"
#include "draw_control.h"
#include "draw_focus.h"
#include "draw_scroll.h"
#include "pointl.h"
#include "setting.h"

using namespace codescan;
using namespace draw;
using namespace draw::controls;

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
static const char* line_feed = "\r\n";

class widget_codeview : public control, vector<char> {
	struct snipet {
		const char*		match;
		const char*		before;
		const char*		after;
	};
	const char* url = 0;
	int	cash_origin = -1;
	int	lines_per_page = 0;
	int	p1 = 0, p2 = 0;
	int	horiz_position = -1;
	pointl pos1 = {}, pos2 = {}, size = {};
	pointl origin = {}, maximum = {};
	rect rctext = {4, 4, 4, 4};
	bool readonly = false, modified = false;

	bool ismodified() const {
		return modified;
	}
	void ensurevisible(int v) {
		if(v < 0 || v >= maximum.y)
			return;
		if(v < origin.y) {
			origin.y = v;
			invalidate();
		}
		if(v > origin.y + (lines_per_page - 1)) {
			origin.y = v - (lines_per_page - 1);
			invalidate();
		}
	}
	void invalidate() {
		cash_origin = -1;
	}
	static bool iswhitespace(char s) {
		return s == 9 || s == 32 || s == 10 || s == 13;
	}
	static bool isidentifier(char sym) {
		return ischa(sym) || sym == '_' || (sym >= '0' && sym <= '9');
	}
	static const char* next(const char* p) {
		if(*p == 10) {
			p++;
			if(*p == 13)
				p++;
		} else if(*p == 13) {
			p++;
			if(*p == 10)
				p++;
		} else if(*p)
			p++;
		return p;
	}
	static const char* prev(const char* pb, const char* p) {
		if(p <= pb)
			return p;
		else if(p[-1] == 10) {
			p--;
			if(p > pb && p[-1] == 13)
				p--;
		} else if(p[-1] == 13) {
			p--;
			if(p > pb && p[-1] == 10)
				p--;
		} else
			p--;
		return p;
	}
	static const char* linee(const char* p) {
		while(*p && *p != 10 && *p != 13)
			p++;
		return p;
	}
	static const char* lineb(const char* pb, const char* p) {
		while(p > pb && p[-1] != 10 && p[-1] != 13)
			p--;
		return p;
	}
	int	hittest(rect rc, point pt, unsigned state) const {
		return draw::hittest({rc.x1, rc.y1 - origin.y, rc.x2, rc.y2}, ptr(0), state, pt);
	}
	const char* getcurrent() const {
		return (char*)data + p1;
	}
	const char* getstart() const {
		return (char*)data;
	}
	char* getbegin() const {
		if(p2 == -1)
			return (char*)data + p1;
		return (char*)data + imin(p1, p2);
	}
	char* getend() const {
		if(p2 == -1)
			return (char*)data + p1;
		return (char*)data + imax(p1, p2);
	}
	pointl getbeginpos() const {
		if(p2 == -1)
			return pos1;
		if(p1 < p2)
			return pos1;
		return pos2;
	}
	pointl getendpos() const {
		if(p2 == -1)
			return pos1;
		if(p1 > p2)
			return pos1;
		return pos2;
	}
	pointl getcurrentpos() const {
		if(horiz_position != -1)
			return {horiz_position, pos1.y};
		return pos1;
	}
	int	getindex(pointl pt) const {
		return codescan::getindex(begin(), pt);
	}
	bool isselected() const {
		return p2 != -1 && p1 != -1;
	}
	void updatetype(group_s& type, const char* sym, unsigned size) const {
		//if(!package)
		//	return;
		//for(auto& e : package->symbols) {
		//	if(e.parent != code::Class && e.parent != code::This)
		//		continue;
		//	auto pn = package->getsymstr(&e - package->symbols.begin());
		//	auto sz = zlen(pn);
		//	if(sz == size && memcmp(pn, sym, sz) == 0) {
		//		if(e.parent == code::Class)
		//			type = Type;
		//		else if(e.is(code::Const))
		//			type = Constant;
		//		else
		//			type = Member;
		//		return;
		//	}
		//}
	}
	static void post_right() {
		auto p = (widget_codeview*)hot.object;
		p->right(hot.param != 0, hot.param2 != 0);
		p->ensurevisible(p->pos1.y + 1);
	}
	static void post_left() {
		auto p = (widget_codeview*)hot.object;
		p->left(hot.param != 0, hot.param2 != 0);
		p->ensurevisible(p->pos1.y - 1);
	}
	static void post_set_horiz() {
		auto p = (widget_codeview*)hot.object;
		p->set(hot.param, hot.param2 != 0, true);
	}
	static void post_set() {
		auto p = (widget_codeview*)hot.object;
		p->set(hot.param, hot.param2 != 0, false);
	}
	static void post_set_clear() {
		auto p = (widget_codeview*)hot.object;
		p->set(hot.param, hot.param2 != 0, true);
		p->clear();
	}
	static void post_clear() {
		auto p = (widget_codeview*)hot.object;
		p->clear();
	}
	static void post_paste_symbol() {
		char temp[8];
		auto p = (widget_codeview*)hot.object;
		p->paste(szput(temp, hot.param));
		p->pastesnipets();
	}
	static void post_paste_cr() {
		auto p = (widget_codeview*)hot.object;
		p->pastecr();
	}
	static void post_wordselect() {
		auto p = (widget_codeview*)hot.object;
		p->left(false, true);
		p->right(true, true);
	}
	void redraw(const rect& rco) {
		if(!fontsize.x || !fontsize.y)
			return;
		auto push_font = font;
		auto push_fore = fore;
		draw::font = default_font;
		auto focused = isfocused(this);
		rect rc = rco + rctext;
		// Mouse input handle
		rect r1 = rc;
		rc.y1 -= origin.y * fontsize.y;
		rc.x1 -= origin.x;
		auto x = rc.x1, y = rc.y1;
		if(ishilite(r1)) {
			pointl mpos;
			mpos.x = (hot.mouse.x - r1.x1 + origin.x + fontsize.x / 2) / fontsize.x;
			mpos.y = (hot.mouse.y - r1.y1) / fontsize.y + origin.y;
			auto x1 = rc.x1 + mpos.x * fontsize.x;
			auto y1 = rc.y1 + mpos.y * fontsize.y;
			rect r2 = {x1, y1, x1 + fontsize.x, y1 + fontsize.y};
			ishilite(r2);
			switch(hot.key) {
			case MouseLeft:
			case MouseLeft | Shift:
				if(hot.pressed) {
					auto i = getindex(mpos);
					draw::execute(post_set, i, (hot.key & Shift) != 0, this);
				}
				break;
			case MouseMove:
				if(hot.pressed) {
					auto i = getindex(mpos);
					draw::execute(post_set, i, 1, this);
				}
				break;
			case MouseLeftDBL:
				if(hot.pressed)
					draw::execute(post_wordselect, 0, 0, this);
				break;
			}
		}
		pointl pos = {};
		group_s type = IllegalSymbol;
		const char* ps = begin();
		while(true) {
			auto x1 = x + pos.x * fontsize.x;
			auto y1 = y + pos.y * fontsize.y;
			auto pb = ps;
			ps = codescan::getnext(ps, pos.x, pos.y, type);
			if(pb == ps)
				break;
			if(type == Identifier)
				updatetype(type, pb, ps - pb);
			auto& ei = bsdata<groupi>::elements[type];
			if(type != WhiteSpace) {
				fore = ei.fore;
				text(x1, y1, pb, ps - pb, ei.flags);
			}
		}
		// Draw hilite
		if(focused) {
			auto x1 = x + pos1.x * fontsize.x;
			auto y1 = y + pos1.y * fontsize.y;
			line(x1, y1, x1, y1 + fontsize.y, colors::text.mix(colors::active));
		}
		auto draw_selecton = (p1 != -1);
		if(draw_selecton) {
			auto p1 = getbeginpos();
			auto p2 = getendpos();
			for(auto i = p1.y; i <= p2.y; i++) {
				auto x1 = rco.x1;
				auto x2 = rco.x2;
				auto y1 = y + i * fontsize.y;
				if(i == p1.y)
					x1 = x + p1.x * fontsize.x;
				if(i == p2.y)
					x2 = x + p2.x * fontsize.x;
				rectf({x1, y1, x2, y1 + fontsize.y}, colors::active, 128);
			}
		}
		font = push_font;
		fore = push_fore;
		if(focused) {
			switch(hot.key) {
			case KeyRight:
			case KeyRight | Shift:
			case KeyRight | Ctrl:
			case KeyRight | Shift | Ctrl:
				draw::execute(post_right, (hot.key & Shift) != 0, (hot.key & Ctrl) != 0, this);
				break;
			case KeyLeft:
			case KeyLeft | Shift:
			case KeyLeft | Ctrl:
			case KeyLeft | Shift | Ctrl:
				draw::execute(post_left, (hot.key & Shift) != 0, (hot.key & Ctrl) != 0, this);
				break;
			case KeyUp:
			case KeyUp | Shift:
				if(getcurrentpos().y > 0) {
					auto pt = getcurrentpos();
					pt.y -= 1;
					draw::execute(post_set_horiz, getindex(pt), (hot.key & Shift) != 0, this);
				}
				break;
			case KeyDown:
			case KeyDown | Shift:
				if(getcurrentpos().y < maximum.y) {
					auto pt = getcurrentpos();
					pt.y += 1;
					draw::execute(post_set_horiz, getindex(pt), (hot.key & Shift) != 0, this);
				}
				break;
			case KeyPageDown:
			case KeyPageDown | Shift:
				if(true) {
					auto pt = getcurrentpos();
					auto n = origin.y + lines_per_page - 1;
					if(pt.y != n)
						pt.y = n;
					else
						pt.y += lines_per_page - 1;
					draw::execute(post_set_horiz, getindex(pt), (hot.key & Shift) != 0, this);
				}
				break;
			case KeyPageUp:
			case KeyPageUp | Shift:
				if(true) {
					auto pt = getcurrentpos();
					auto n = origin.y;
					if(pt.y != n)
						pt.y = n;
					else
						pt.y -= lines_per_page - 1;
					draw::execute(post_set_horiz, getindex(pt), (hot.key & Shift) != 0, this);
				}
				break;
			case KeyBackspace:
				if(!readonly) {
					if(!isselected()) {
						auto pb = getstart();
						auto ps = getcurrent();
						draw::execute(post_set_clear, prev(pb, ps) - pb, 1, this);
					} else
						draw::execute(post_clear, 0, 0, this);
				}
				break;
			case KeyDelete:
				if(!readonly) {
					if(!isselected()) {
						auto pb = getstart();
						auto ps = getcurrent();
						draw::execute(post_set_clear, next(ps) - pb, 1, this);
					} else
						draw::execute(post_clear, 0, 0, this);
				}
				break;
			case InputSymbol:
				if(hot.param >= 0x20 && !readonly)
					draw::execute(post_paste_symbol, hot.param, 0, this);
				break;
			case KeyEnter:
				if(!readonly)
					draw::execute(post_paste_cr, 0, 0, this);
				break;
			case KeyHome:
			case KeyHome | Shift:
				if(true) {
					auto pb = getstart();
					auto ps = getcurrent();
					auto pa = lineb(pb, ps);
					auto pp = skipsp(pa);
					if(ps == pa)
						break;
					if(ps == pp)
						ps = pa;
					else
						ps = pp;
					draw::execute(post_set, ps - pb, (hot.key & Shift) != 0, this);
				}
				break;
			case KeyEnd:
			case KeyEnd | Shift:
				if(true) {
					auto pb = getstart();
					auto ps = getcurrent();
					ps = linee(ps);
					draw::execute(post_set, ps - pb, (hot.key & Shift) != 0, this);
				}
				break;
			case KeyTab:
				if(!readonly)
					draw::execute(post_paste_symbol, '\t', 0, this);
				break;
			default: break;
			}
		}
	}
	int	getpixelperline() const {
		return fontsize.y;
	}
	void updateposition() {
		if(cash_origin == -1) {
			getstate(getstart(), origin.y, cash_origin, size, p1, pos1, p2, pos2);
			maximum.x = size.x * fontsize.x;
			maximum.y = size.y;
		}
	}
	void paint(const rect& rc) override {
		auto pixels_per_line = getpixelperline();
		if(!pixels_per_line)
			return;
		updateposition();
		lines_per_page = rc.height() / pixels_per_line;
		draw::scroll scrollv(origin.y, lines_per_page, maximum.y, rc); scrollv.input();
		draw::scroll scrollh(origin.x, rc.width(), maximum.x, rc, true); scrollh.input();
		control::paint(rc);
		auto push_clip = clipping;
		setclip(rc);
		redraw(rc);
		clipping = push_clip;
		scrollv.view(isfocused());
		scrollh.view(isfocused());
	}
	void set(int index, bool shift, bool keephoriz = false) {
		if(index < 0)
			index = 0;
		else if(index > (int)getcount())
			index = getcount();
		if(shift) {
			if(p2 == -1)
				p2 = p1;
		} else
			p2 = -1;
		p1 = index;
		if(keephoriz) {
			if(horiz_position == -1)
				horiz_position = pos1.x;
		} else
			horiz_position = -1;
		invalidate();
		updateposition();
	}
	void clear() {
		if(p2 != -1 && p1 != p2 && data) {
			auto i1 = imin(p1, p2);
			auto i2 = imax(p1, p2);
			memcpy(ptr(i1), ptr(i2), (getcount() - i2 + 1) * sizeof(char));
			count -= (i2 - i1);
			invalidate();
			if(p1 > p2)
				p1 = p2;
			modified = true;
		}
		p2 = -1;
	}
	void paste(const char* input) {
		clear();
		auto lenght = zlen(input);
		reserve(count + lenght + 1);
		memmove((char*)data + p1 + lenght, (char*)data + p1, count - p1 + 1);
		memcpy((char*)data + p1, input, lenght);
		count += lenght;
		((char*)data)[count] = 0;
		set(p1 + lenght, false);
		modified = true;
	}
	bool isnextlevel(const char* p) const {
		//if(!lex)
		//	return false;
		auto pb = getstart();
		if(p <= pb)
			return false;
		//if(lex->increase && zchr(lex->increase, p[-1]))
		//	return true;
		//if(p[-1] == lex->statement.open)
		//	return true;
		return false;
	}
	void pastecr() {
		auto pb = getstart();
		auto pc = getcurrent();
		auto p = lineb(pb, pc);
		char temp[260]; stringbuilder sb(temp);
		sb.add(line_feed);
		while(*p == 32 || *p == 9)
			sb.add(*p++);
		if(isnextlevel(pc))
			sb.add(9);
		paste(temp);
	}
	void pastesp(const char* value, const char* start_line) {
		if(!value || value[0] == 0)
			return;
		char temp[2048]; stringbuilder sb(temp);
		for(auto p = value; *p; p++) {
			if(*p == '\n') {
				auto pb = getstart();
				auto pc = getcurrent();
				sb.add(line_feed);
				auto pn = start_line;
				while(*pn == 32 || *pn == 9)
					sb.add(*pn++);
				paste(temp);
				sb.clear();
			} else
				sb.add(*p);
		}
		if(sb)
			paste(temp);
	}
	bool pastetemplate(const char* match, const char* before, const char* after) {
		auto pb = getstart();
		auto pc = getcurrent();
		auto sz = zlen(match);
		auto ps = pc - sz;
		if(ps <= pb)
			return false;
		if(memcmp(ps, match, sz) != 0)
			return false;
		auto pn = lineb(pb, pc);
		pastesp(before, pn);
		auto index = p1;
		pastesp(after, pn);
		set(index, false);
		return true;
	}
	void left(bool shift, bool ctrl) {
		auto pb = getstart();
		auto p = getcurrent();
		if(!ctrl)
			p = prev(pb, p);
		else if(p > pb) {
			if(iswhitespace(p[-1])) {
				while(p > pb && iswhitespace(p[-1]))
					p = prev(pb, p);
			} else {
				auto result = isidentifier(p[-1]);
				while(p > data && !iswhitespace(p[-1]) && isidentifier(p[-1]) == result)
					p = prev(pb, p);
			}
		}
		set(p - begin(), shift);
	}
	void right(bool shift, bool ctrl) {
		const char* p = ptr(p1);
		if(!ctrl)
			p = next(p);
		else {
			auto pe = end();
			if(iswhitespace(*p)) {
				while(p < pe && iswhitespace(*p))
					p = next(p);
			} else {
				auto result = isidentifier(*p);
				while(p < pe && !iswhitespace(*p) && isidentifier(*p) == result)
					p = next(p);
			}
		}
		set(p - begin(), shift);
	}
	void pastesnipets() {
		static snipet snipets[] = {
			//{"if(", "", ") {\n\t\n}"},
			//{"for(", "", "; ; ) {\n\t\n}"},
			{"`", "", "`"},
			{"\"", "", "\""},
			{"\'", "", "\'"},
			{"[", "", "]"},
			{"(", "", ")"},
			{"{", "\n\t", "\n}"},
		};
		for(auto& e : snipets) {
			if(pastetemplate(e.match, e.before, e.after))
				break;
		}
	}
	bool copy(bool run) {
		if(!isselected())
			return false;
		if(run) {
			auto s1 = getbegin();
			auto s2 = getend();
			clipboard::copy(s1, s2 - s1);
		}
		return true;
	}
	bool paste(bool run) {
		if(readonly)
			return false;
		if(run) {
			clear();
			auto p = clipboard::paste();
			if(p)
				paste(p);
			delete p;
		}
		return true;
	}
	bool cut(bool run) {
		if(readonly)
			return false;
		if(!isselected())
			return false;
		if(run) {
			auto s1 = getbegin();
			auto s2 = getend();
			clipboard::copy(s1, s2 - s1);
			clear();
		}
		return true;
	}
public:
	const char* getvalue(const char* id, stringbuilder& sb) const override {
		if(equal(id, "URL"))
			return url;
		return control::getvalue(id, sb);
	}
	void setvalue(const char* id, long value) override {
		if(equal(id, "Readonly"))
			readonly = (value != 0);
		else if(equal(id, "URL"))
			open((const char*)value);
		else
			control::setvalue(id, value);
	}
	bool open(const char* url) {
		int s = 0;
		auto p = loadt(url, &s);
		if(!p)
			return false;
		this->url = szdup(url);
		reserve(s + 1);
		setcount(s);
		memcpy(begin(), p, s + 1);
		delete p;
		invalidate();
		return true;
	}
};

static class widget_codeview_plugin : controls::control::plugin, controls::control::plugin::builder {
	builder* getbuilder() override {
		return this;
	}
	bool canopen(const char* url) const override {
		auto ext = szext(url);
		if(equal(ext, "c2"))
			return true;
		return false;
	}
	control* create(const char* url) override {
		auto p = new widget_codeview();
		p->open(url);
		return p;
	}
	void getextensions(stringbuilder& sb) const override {
		const char* exts = "*.c2";
		sb.add("»сходный код (%1)", exts);
		sb.addsz();
		sb.add(exts);
		sb.addsz();
	}
public:
	widget_codeview_plugin() : plugin("Codeview", dock::Workspace) {}
} plugin_instance;

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