#include "draw.h"
#include "draw_button.h"
#include "draw_clipboard.h"
#include "draw_input.h"
#include "draw_focus.h"
#include "draw_scroll.h"
#include "draw_valuelist.h"
#include "io_stream.h"
#include "lexer.h"
#include "handler.h"
#include "package.h"
#include "pointl.h"
#include "setting.h"

using namespace code;
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
	{"Illegal symbol"},
	{"White space"},
	{"Operator"},
	{"Keyword", {}, TextBold},
	{"Comment"},
	{"Number"},
	{"String"},
	{"Identifier"},
	{"Type"},
	{"BlockBegin"}, {"BlockEnd"},
	{"IndexBegin"}, {"IndexEnd"},
	{"ExpressionBegin"}, {"ExpressionEnd"},
};
assert_enum(groupi, ExpressionEnd)

static const sprite* default_font = (sprite*)loadb("art/fonts/code.pma");
static point fontsize;
static const char* line_feed = "\r\n";

class widget_codeview : public control, vector<char> {
	struct snipet {
		const char*	match;
		const char*	before;
		const char*	after;
	};
	const lexer* source_lexer = 0;
	const char* url = 0;
	const char* url_module = 0;
	int	cash_origin = -1;
	int	lines_per_page = 0;
	int	p1 = 0, p2 = 0;
	int	horiz_position = -1;
	pointl pos1 = {}, pos2 = {}, size = {};
	pointl origin = {}, maximum = {};
	rect rctext = {4, 4, 4, 4};
	rect ddclient = {};
	bool readonly = false, modified = false;
	valuelist dropdown;

	bool ismodified() const {
		return modified;
	}
	void cashing() {
		pointl pos;
		pos1 = pos2 = pos = {};
		cash_origin = pos.y;
		auto pb = (const char*)begin();
		auto p = pb;
		while(*p) {
			switch(*p) {
			case 10: case 13:
				if(size.x < pos.x)
					size.x = pos.x;
				p = getnext(p, pos);
				if(origin.y == pos.y)
					cash_origin = p - pb;
				break;
			case 9:
				p = getnext(p, pos);
				break;
			default:
				pos.x++; p++;
				break;
			}
			auto n = p - pb;
			if(p1 == n)
				pos1 = pos;
			if(p2 == n)
				pos2 = pos;
		}
		if(size.x < pos.x)
			size.x = pos.x;
		size.y = pos.y;
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
		return code::getindex(begin(), pt);
	}
	bool isselected() const {
		return p2 != -1 && p1 != -1;
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
		p->ensurevisible(p->pos1.y);
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
	static void post_close_dropdown() {
		auto p = (widget_codeview*)hot.object;
		p->dropdown.clear();
	}
	static void post_choose_dropdown() {
		auto p = (widget_codeview*)hot.object;
		auto n = p->dropdown.getcurrentname();
		p->paste(n);
		p->pastesnipets();
		p->dropdown.clear();
	}
	void setdropdown(int x, int y, int width) {
		dropdown.pixels_per_line = dropdown.getrowheight();
		auto n = dropdown.getmaximum();
		if(n > 10)
			n = 10;
		auto h = n * dropdown.pixels_per_line;
		ddclient.set(x, y, x + 300, y + h);
	}
	void autocomplete() {
		if(!source_lexer)
			return;
		dropdown.always_focus = true;
		dropdown.clear();
		for(auto& e : source_lexer->keywords)
			dropdown.add(e.begin());
		setdropdown(
			client.x1 + pos1.x * fontsize.x - origin.x,
			client.y1 + (pos1.y - origin.y) * fontsize.y + fontsize.y,
			300);
	}
	bool isdropdown() const {
		return dropdown.getmaximum();
	}
	void paint() const override {
		if(!fontsize.x || !fontsize.y)
			return;
		auto package = getpackage();
		auto push_font = font;
		auto push_fore = fore;
		draw::font = default_font;
		auto focused = isfocused();
		rect rc = client + rctext;
		// Mouse input handle
		rect r1 = rc;
		rc.y1 -= origin.y * fontsize.y;
		rc.x1 -= origin.x;
		auto x = rc.x1, y = rc.y1;
		if(ishilite(r1) && (!isdropdown() || !hot.mouse.in(dropdown.client))) {
			pointl mpos;
			mpos.x = (hot.mouse.x - r1.x1 + origin.x + fontsize.x / 2) / fontsize.x;
			mpos.y = (hot.mouse.y - r1.y1) / fontsize.y + origin.y;
			auto x1 = rc.x1 + mpos.x * fontsize.x;
			auto y1 = rc.y1 + mpos.y * fontsize.y;
			switch(hot.key) {
			case MouseLeft:
			case MouseLeft | Shift:
				if(isdropdown()) {
					if(ishilite(dropdown.client))
						break;
					else
						draw::execute(post_close_dropdown, 0, 0, this);
				}
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
			hot.cursor = cursor::Edit;
		}
		pointl pos = {};
		auto type = IllegalSymbol;
		const char* ps = begin();
		while(true) {
			auto x1 = x + pos.x * fontsize.x;
			auto y1 = y + pos.y * fontsize.y;
			auto pb = ps;
			ps = code::getnext(ps, pos, type, source_lexer);
			if(pb == ps)
				break;
			if(source_lexer) {
				if(type == Identifier) {
					if(package) {
						auto sh = package->find(pb, ps-pb);
						if(sh != None) {
							if(package->find(sh, Class) != None)
								type = Type;
						}
					}
				}
			}
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
				auto x1 = client.x1;
				auto x2 = client.x2;
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
				if(isdropdown())
					break;
				if(getcurrentpos().y > 0) {
					auto pt = getcurrentpos();
					pt.y -= 1;
					draw::execute(post_set_horiz, getindex(pt), (hot.key & Shift) != 0, this);
				}
				break;
			case KeyDown:
			case KeyDown | Shift:
				if(isdropdown())
					break;
				if(getcurrentpos().y < maximum.y) {
					auto pt = getcurrentpos();
					pt.y += 1;
					draw::execute(post_set_horiz, getindex(pt), (hot.key & Shift) != 0, this);
				}
				break;
			case KeyPageDown:
			case KeyPageDown | Shift:
				if(!isdropdown()) {
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
				if(!isdropdown()) {
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
				if(readonly)
					break;
				if(isdropdown())
					draw::execute(post_choose_dropdown, 0, 0, this);
				else
					draw::execute(post_paste_cr, 0, 0, this);
				break;
			case KeyHome:
			case KeyHome | Shift:
				if(!isdropdown()) {
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
				if(!isdropdown()) {
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
			case KeyEscape:
				if(isdropdown())
					draw::execute(post_close_dropdown, 0, 0, this);
				break;
			case InputUpdate:
				if(isdropdown())
					draw::execute(post_close_dropdown, 0, 0, this);
				break;
			default: break;
			}
			if(!source_lexer)
				statuscolumn(0, 50, "Text");
			else
				statuscolumn(0, 50, source_lexer->id);
			statuscolumn(1, 160, "%Row: %2i, %Column: %1i", pos1.x, pos1.y);
		}
	}
	int	getpixelperline() const {
		return fontsize.y;
	}
	void paintnc() override {
		auto pixels_per_line = getpixelperline();
		if(!pixels_per_line)
			return;
		if(!lines_per_page) {
			lines_per_page = client.height() / pixels_per_line;
			correction();
		}
		draw::scroll scrollv(origin.y, lines_per_page, maximum.y, client); scrollv.input();
		draw::scroll scrollh(origin.x, client.width(), maximum.x, client, true); scrollh.input();
		control::paint();
		auto push_clip = clipping;
		setclip(client);
		paint();
		clipping = push_clip;
		if(dropdown.getmaximum())
			dropdown.view(ddclient, true, true, false, true);
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
		correction();
	}
	void clear() {
		if(p2 != -1 && p1 != p2 && data) {
			auto i1 = imin(p1, p2);
			auto i2 = imax(p1, p2);
			memcpy(ptr(i1), ptr(i2), (getcount() - i2 + 1) * sizeof(char));
			count -= (i2 - i1);
			if(p1 > p2)
				p1 = p2;
			correction();
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
		if(!source_lexer)
			return false;
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
	bool matchl(const char* v) const {
		if(!v)
			return false;
		auto n = zlen(v);
		auto pc = getcurrent() - n;
		if(begin() > pc)
			return false;
		return szmatch(pc, v);
	}
	void ensurevisible(int y) {
		if(y < origin.y)
			origin.y = y;
		if(y >= origin.y + lines_per_page)
			origin.y = y - lines_per_page + 1;
		if(origin.y + lines_per_page >= maximum.y)
			origin.y = maximum.y - lines_per_page + 1;
		if(origin.y < 0)
			origin.y = 0;
		correction();
	}
	void correction() {
		cashing();
		maximum.x = size.x * fontsize.x;
		maximum.y = size.y;
	}
	package* getpackage() const {
		auto p = package::findmodule(url_module);
		if(!p)
			return 0;
		return p;
	}
public:
	const char* getvalue(const char* id, stringbuilder& sb) const override {
		if(equal(id, "URL"))
			return url;
		else if(equal(id, "URLPath"))
			return url;
		return control::getvalue(id, sb);
	}
	void copy() {
		auto p1 = getbegin(), p2 = getend();
		auto sn = p2 - p1;
		if(sn > 0)
			clipboard::copy(p1, sn);
	}
	void paste() {
		long size;
		auto p = clipboard::paste(&size);
		if(!p)
			return;
		clear();
		paste(p);
		delete p;
	}
	void parsecurrent(package* p) {
		this_pack = p;
		core.clear();
		core.context = getcurrent();
		parse(getbegin());
	}
	void readpackage() {
		url_module = package::getmodule(url);
		if(!url_module || !source_lexer)
			return;
		source_lexer->setgrammar();
		auto p = package::addmodule(url_module);
		p->addclasses(source_lexer->standart_classes);
		parsecurrent(p);
	}
	bool execute(const char* id, bool run) override {
		if(equal(id, "Edit")) {
			if(readonly)
				return false;
		} else if(equal(id, "Parser")) {
			return true;
		} else if(equal(id, "Cut")) {
			if(!isselected() || readonly)
				return false;
			if(run) {
				copy();
				clear();
			}
		} else if(equal(id, "Copy")) {
			if(!isselected())
				return false;
			if(run)
				copy();
		} else if(equal(id, "Paste")) {
			if(!clipboard::isallowpaste() || readonly)
				return false;
			if(run)
				paste();
		} else if(equal(id, "Save")) {
			if(run) {
				io::file file(url, StreamText | StreamWrite);
				if(!file)
					return false;
				file << (char*)data;
				modified = false;
			}
		} else if(equal(id, "Open")) {
			if(run) {
				int s = 0;
				auto p = loadt(url, &s);
				if(!p)
					return false;
				reserve(s + 1);
				setcount(s);
				memcpy(begin(), p, s + 1);
				delete p;
				correction();
				source_lexer = findlexer(szext(url));
				readpackage();
				update_codetree();
			}
		} else if(equal(id, "ParseAll")) {
			auto package = getpackage();
			if(!package)
				return false;
			if(run) {
				source_lexer->setgrammar();
				parsecurrent(package);
				update_codetree();
			}
		} else if(equal(id, "SelectAll")) {
			if(run) {
				set(0, false, false);
				set(zlen((const char*)data), true, false);
			}
		} else if(equal(id, "Autocomplete")) {
			if(run)
				autocomplete();
		} else
			return false;
		return true;
	}
	void setvalue(const char* id, long value) override {
		if(equal(id, "Readonly"))
			readonly = (value != 0);
		else if(equal(id, "Modified"))
			modified = (value != 0);
		else if(equal(id, "URL"))
			url = szdup((const char*)value);
		else
			control::setvalue(id, value);
	}
	static lexer* findlexer(const char* ext) {
		char temp[260]; stringbuilder sb(temp);
		sb.add("*.%1", ext);
		for(auto& e : bsdata<lexer>()) {
			if(szpmatch(temp, e.extensions))
				return &e;
		}
		return 0;
	}
	const char** getcommands(const char* id) const override {
		static const char* edit_commands[] = {"Cut", "Copy", "Paste", "SelectAll", 0};
		static const char* parser_commands[] = {"ParseAll", "Autocomplete", 0};
		if(equal(id, "Parser"))
			return parser_commands;
		if(equal(id, "Edit"))
			return edit_commands;
		return 0;
	}
	const char** getcommands() const override {
		static const char* source[] = {"#Edit", "#Parser", 0};
		return source;
	}
};

static class widget_codeview_plugin : controls::plugin, controls::plugin::builder {
	plugin::builder* getbuilder() override {
		return this;
	}
	bool canopen(const char* url) const override {
		auto ext = szext(url);
		if(widget_codeview::findlexer(ext))
			return true;
		return false;
	}
	control* create() override {
		return new widget_codeview();
	}
	void getextensions(stringbuilder& sb) const override {
		for(auto& e : bsdata<lexer>()) {
			sb.add("Исходный код (%1)", e.id);
			sb.addsz();
			sb.add(e.extensions);
			sb.addsz();
		}
	}
public:
	widget_codeview_plugin() : plugin("Codeview", dock::Workspace) {}
} plugin_instance;

static const setting::element colors_editor[] = {
	{"IdentifierColor", bsdata<groupi>::elements[Identifier].fore},
	{"KeywordColor", bsdata<groupi>::elements[Keyword].fore},
	{"TypeColor", bsdata<groupi>::elements[Type].fore},
	{"CommentColor", bsdata<groupi>::elements[Comment].fore},
	{"StringColor", bsdata<groupi>::elements[String].fore},
	{"NumberColor", bsdata<groupi>::elements[Number].fore},
	{"OperatorColor", bsdata<groupi>::elements[Operator].fore},
};
static setting::element format_setting[] = {{"Tabulation", {common_padding}},
};
static setting::element project_urls_general[] = {
	{"Project", {urls::project, setting::Url}},
	{"Projects", {urls::projects, setting::Url}},
	{"Library", {urls::library, setting::Url}},
};
static setting::header headers[] = {
	{"Project", "General", "URLs", project_urls_general},
	{"Editor", "General", "Formating", format_setting},
	{"Colors", "Editor", 0, colors_editor},
};

static void update_theme_colors() {
	bsdata<groupi>::elements[BlockBegin].fore = bsdata<groupi>::elements[Operator].fore;
	bsdata<groupi>::elements[BlockEnd].fore = bsdata<groupi>::elements[Operator].fore;
	bsdata<groupi>::elements[IndexBegin].fore = bsdata<groupi>::elements[Operator].fore;
	bsdata<groupi>::elements[IndexEnd].fore = bsdata<groupi>::elements[Operator].fore;
	bsdata<groupi>::elements[ExpressionBegin].fore = bsdata<groupi>::elements[Operator].fore;
	bsdata<groupi>::elements[ExpressionEnd].fore = bsdata<groupi>::elements[Operator].fore;
}

static void update_theme() {
	bsdata<groupi>::elements[IllegalSymbol].fore = colors::red;
	bsdata<groupi>::elements[WhiteSpace].fore = colors::red;
	bsdata<groupi>::elements[Identifier].fore = colors::text;
	bsdata<groupi>::elements[Operator].fore = colors::text;
	if(colors::window.isdark()) {
		bsdata<groupi>::elements[Type].fore = color(78, 201, 176);
		bsdata<groupi>::elements[Keyword].fore = color(86, 156, 214);
		bsdata<groupi>::elements[String].fore = color(214, 136, 82);
		bsdata<groupi>::elements[Number].fore = color(184, 215, 163);
		bsdata<groupi>::elements[Comment].fore = color(87, 166, 74);
	} else {
		bsdata<groupi>::elements[Type].fore = color(78, 201, 176);
		bsdata<groupi>::elements[Keyword].fore = color(86, 156, 214);
		bsdata<groupi>::elements[String].fore = color(214, 136, 82);
		bsdata<groupi>::elements[Number].fore = color(184, 215, 163);
		bsdata<groupi>::elements[Comment].fore = color(0, 128, 0);
	}
}

void initialize_codeview() {
	auto old_font = draw::font;
	draw::font = default_font;
	fontsize.x = draw::textw('A');
	fontsize.y = draw::texth();
	draw::font = old_font;
}

HANDLER(after_theme_change) {
	update_theme();
	update_theme_colors();
}

HANDLER(after_initialize) {
	update_theme_colors();
}
