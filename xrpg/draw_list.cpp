#include "crt.h"
#include "draw.h"
#include "draw_control.h"
#include "draw_focus.h"
#include "draw_scroll.h"
#include "handler.h"

using namespace draw;
using namespace draw::controls;

namespace metrics {
extern int				edit;
}

const sprite*			list::std_tree_images = (sprite*)loadb("art/pma/tree.pma");
static char				search_text[32];
static unsigned			search_time;
static stringbuilder	search_sb(search_text);
int						list::current_hilite_treemark;
int						list::current_hilite_row;
int						list::current_hilite_column;

void list::beforemodal() {
	list::current_hilite_row = -1;
	list::current_hilite_treemark = -1;
}

void list::ensurevisible() {
	correction();
	if(current < origin)
		origin = current;
	if(current > origin + page - 1)
		origin = current - page + 1;
}

void list::select(int index, int column) {
	current = index;
	ensurevisible();
}

void list::correction() {
	auto maximum = getmaximum();
	if(current >= maximum)
		current = maximum - 1;
	if(current < 0)
		current = 0;
	if(page) {
		if(origin > maximum - page)
			origin = maximum - page;
		if(origin < 0)
			origin = 0;
	}
	auto maximum_width = getmaximumwidth();
	if(page_x) {
		if(origin_x > maximum_width - page_x)
			origin_x = maximum_width - page_x;
		if(origin_x < 0)
			origin_x = 0;
	}
}

void list::hilight(const rect& rc) const {
	auto push_fore = fore;
	auto focused = isfocused();
	const color c1 = focused ? colors::button : colors::button.mix(colors::window, 192);
	fore = c1;
	//rectf(r1);
	//rectb(r1);
	fore = push_fore;
	if(focused)
		rectfocus();
}

void list::rowhilite(const rect& rc, int index) const {
	if(show_selection) {
		auto a = ishilite({rc.x1, rc.y1, rc.x2 - 1, rc.y2 - 1});
		if(index == current)
			hilight(rc);
		else if(a && index == current_hilite_row) {
			auto push_fore = fore;
			auto push_alpha = alpha;
			alpha = 64;
			fore = colors::button;
			rectf();
			alpha = push_alpha;
			fore = push_fore;
		}
	}
}

void list::row(const rect& rc, int line) const {
	auto r1 = rc; rowhilite(r1, line);
	auto i = getrowimage(line);
	if(i != -1) {
		auto w = r1.height();
		draw::image(r1.x1 + w / 2, r1.y1 + w / 2, gettreeimages(), i, 0);
		r1.x1 += w + 1;
	}
	char temp[260]; stringbuilder sb(temp);
	auto p = getname(line, 0, sb);
	if(p) {
		auto push_width = width;
		auto push_caret = caret;
		caret.x = r1.x1 + metrics::edit;
		caret.y = r1.y1 + metrics::edit;
		width = r1.width() - metrics::edit * 2;
		textc(p);
		caret = push_caret;
		width = push_width;
	}
}

int	list::getrowheight() {
	return texth() + metrics::edit * 2;
}

void list::mousehiliting(const rect& client, point mouse) const {
	auto maximum = getmaximum();
	current_hilite_row = origin + (mouse.y - client.y1) / pixels_per_line;
	if(current_hilite_row >= maximum)
		current_hilite_row = -1;
}

bool list::isopen(int index) const {
	return (index < getmaximum() - 1) ? (getlevel(index + 1) > getlevel(index)) : false;
}

void list::treemark(const rect& rc, int line, int level) const {
	if(!isgroup(line))
		return;
	auto isopen = list::isopen(line);
	auto push_caret = caret;
	caret.x = rc.x1 + rc.width() / 2;
	caret.y = rc.y1 + rc.height() / 2 - 1;
	if(ishilite(rc))
		current_hilite_treemark = line;
	circle(6);
	caret = push_caret;
	caret.x -= 4;
	draw::line(caret.x + 4, caret.y);
	if(!isopen) {
		caret.x = push_caret.x;
		caret.y = push_caret.y - 4;
		draw::line(caret.x, caret.y+4);
	}
	caret = push_caret;
}

void list::post_select() {
	auto p = (list*)hot.object;
	auto r = hot.param;
	auto c = hot.param2;
	p->select(r, c);
}

void list::mouseclick() const {
	if(hot.key==MouseLeft && hot.pressed)
		draw::execute(post_select, current_hilite_row, getcolumn(), this);
}

int list::getlinesperpage(int height) const {
	if(!pixels_per_line)
		return 0;
	return height / pixels_per_line;
}

void list::paintrows(const rect& rc) const {
	auto focused = isfocused();
	if(ishilite(rc)) {
		mousehiliting(rc, hot.mouse);
		switch(hot.key) {
		case MouseLeft:
		case MouseRight:
			if(focused) {
				if(current_hilite_treemark != -1) {
					if(hot.key == MouseLeft && hot.pressed)
						post("Toggle");
				} else
					mouseclick();
			}
			break;
		case MouseLeftDBL:
			if(focused)
				post("Choose");
			break;
		}
	}
	auto x1 = rc.x1 - origin_x, y1 = rc.y1;
	auto x2 = rc.x2;
	auto ix = origin;
	auto maximum = getmaximum();
	while(true) {
		if(y1 >= rc.y2)
			break;
		if(ix >= maximum)
			break;
		rect rcm = {x1, y1, x2, y1 + pixels_per_line};
		auto push_fore = fore;
		fore = colors::border;
		if(hilite_odd_lines) {
			if(ix & 1) {
				auto push_alpha = alpha;
				alpha = 16;
				rectf();
				alpha = push_alpha;
			}
		}
		if(show_grid_lines) {
			//line(rc.x1, rcm.y2 - 1, rc.x2, rcm.y2 - 1);
		}
		fore = push_fore;
		row(rcm, ix);
		y1 += pixels_per_line;
		ix++;
	}
}

void list::paintnc() {
	page_x = client.width() - 1;
	if(show_header)
		client.y1 += rowheader(client);
	control::paint();
	client.x1++; client.y1++;
	if(!pixels_per_line)
		pixels_per_line = getrowheight();
	page = getlinesperpage(client.height());
	correction();
	if(!pixels_per_line)
		return;
	auto maximum = getmaximum();
	auto maximum_x = getmaximumwidth();
	scroll scrollv(origin, page, maximum, client, false);
	scroll scrollh(origin_x, page_x, maximum_x, client, true);
	scrollv.input();
	scrollh.input();
	auto push_clip = clipping; setclip(client);
	auto focused = isfocused();
	paint();
	clipping = push_clip;
	scrollv.view(focused);
	scrollh.view(focused);
}

void list::paint() const {
	paintrows(client);
	auto focused = isfocused();
	if(focused) {
		int m;
		blockfocuskeys();
		switch(hot.key) {
		case KeyUp:
			draw::execute(post_select, current - 1, getcolumn(), this);
			break;
		case KeyDown:
			draw::execute(post_select, current + 1, getcolumn(), this);
			break;
		case KeyHome:
			if(current == 0)
				break;
			draw::execute(post_select, 0, getcolumn(), this);
			break;
		case KeyEnd:
			m = getmaximum();
			if(current == m - 1)
				break;
			draw::execute(post_select, m - 1, getcolumn(), this);
			break;
		case KeyPageUp:
			if(current != origin)
				draw::execute(post_select, origin, getcolumn(), this);
			else
				draw::execute(post_select, current - (page - 1), getcolumn(), this);
			break;
		case KeyPageDown:
			if(current != (origin + page - 1))
				draw::execute(post_select, origin + page - 1, getcolumn(), this);
			else
				draw::execute(post_select, current + page - 1, getcolumn(), this);
			break;
		case KeyEnter:
			post("Choose");
			break;
		case InputSymbol:
			if(hot.key >= 0x20) {
				auto time_clock = getcputime();
				if(!search_time || (time_clock - search_time) > 150)
					search_sb.clear();
				search_time = time_clock;
				search_sb.addsym(hot.param);
				auto i1 = find(-1, -1, search_text);
				if(i1 != -1)
					draw::execute(post_select, i1, getcolumn(), this);
			}
			break;
		}
	}
}

int list::find(int line, int column, const char* value, int lenght) const {
	if(line < 0)
		line = getline() + 1;
	if(column == -1)
		column = getcolumn();
	if(lenght == -1)
		lenght = zlen(value);
	auto m = getmaximum();
	while(line < m) {
		char temp[260]; stringbuilder sb(temp);
		auto p = getname(line, column, sb);
		if(p && szcmpi(p, value, lenght) == 0)
			return line;
		line++;
	}
	return -1;
}

int list::getparent(int index) const {
	int level = getlevel(index);
	while(index) {
		if(level > getlevel(index))
			return index;
		index--;
	}
	if(level > getlevel(index))
		return index;
	return -1;
}

int list::getroot(int index) const {
	while(true) {
		auto parent = getparent(index);
		if(parent == -1)
			return index;
		index = parent;
	}
}

int list::getblockcount(int index) const {
	auto start = index;
	auto level = getlevel(index++);
	auto index_last = getmaximum();
	while(index < index_last) {
		if(level >= getlevel(index))
			break;
		index++;
	}
	return index - start;
}

int	list::getnextblock(int index, int increment) const {
	auto i = index;
	auto level = getlevel(i);
	auto maximum = getmaximum();
	while(true) {
		auto n = i + increment;
		if((n < 0) || (n >= maximum))
			return index;
		auto m = getlevel(n);
		if(m == level)
			return n;
		if(m < level)
			return index;
		i = n;
	}
}

void list::toggle(int index) {
	if(!isgroup(index))
		return;
	auto mm = getmaximum();
	auto cc = current;
	if(isopen(index))
		collapse(index);
	else
		expand(index);
	if(cc > index) {
		if(mm < getmaximum())
			current += getmaximum() - mm;
	}
}

void list::expandall(int max_level) {
	for(int level = 1; level <= max_level; level++) {
		bool need_test = true;
		while(need_test) {
			need_test = false;
			auto c = getmaximum();
			for(auto i = 0; i < c; i++) {
				if(level != getlevel(i))
					continue;
				if(i < c - 1) {
					if(getlevel(i + 1) > level)
						continue;
				}
				if(isgroup(i)) {
					auto i1 = getmaximum();
					expand(i);
					if(i1 < getmaximum()) {
						need_test = true;
						break;
					}
				}
			}
		}
	}
}