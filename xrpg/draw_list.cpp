#include "crt.h"
#include "draw_button.h"
#include "draw_control.h"
#include "draw_focus.h"
#include "draw_scroll.h"
#include "handler.h"

using namespace draw;
using namespace draw::controls;

const sprite*			list::std_tree_images = (sprite*)loadb("art/pma/tree.pma");
static char				search_text[32];
static unsigned			search_time;
static stringbuilder	search_sb(search_text);
static int				current_hilite_row;
static int				current_hilite_treemark;

HANDLER(before_modal) {
	current_hilite_row = -1;
	current_hilite_treemark = -1;
}

void list::ensurevisible() {
	correction();
	if(current < origin)
		origin = current;
	if(current > origin + lines_per_page - 1)
		origin = current - lines_per_page + 1;
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
	if(lines_per_page) {
		if(origin > maximum - lines_per_page)
			origin = maximum - lines_per_page;
		if(origin < 0)
			origin = 0;
	}
	auto maximum_width = getmaximumwidth();
	if(pixels_per_width) {
		if(origin_width > maximum_width - pixels_per_width)
			origin_width = maximum_width - pixels_per_width;
		if(origin_width < 0)
			origin_width = 0;
	}
}

void list::hilight(const rect& rc) const {
	auto focused = isfocused(this);
	const color c1 = focused ? colors::button : colors::button.mix(colors::window, 192);
	rect r1 = {rc.x1, rc.y1, rc.x2 - 1, rc.y2 - 1};
	rectf(r1, c1); rectb(r1, c1);
	if(focused)
		rectx(r1, colors::text.mix(colors::form, 200));
}

void list::rowhilite(const rect& rc, int index) const {
	if(show_selection) {
		auto a = ishilite({rc.x1, rc.y1, rc.x2 - 1, rc.y2 - 1});
		if(index == current)
			hilight(rc);
		else if(a && index == current_hilite_row)
			rectf(rc, colors::button.mix(colors::window, 96));
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
	if(p)
		textc(r1.x1 + metrics::edit, r1.y1 + metrics::edit, r1.width() - metrics::edit * 2, p);
}

int	list::getrowheight() {
	return texth() + metrics::edit * 2;
}

void list::mousehiliting(const rect& screen, point mouse) const {
	current_hilite_row = origin + (mouse.y - screen.y1) / pixels_per_line;
}

bool list::isopen(int index) const {
	return (index < getmaximum() - 1) ? (getlevel(index + 1) > getlevel(index)) : false;
}

void list::treemark(const rect& rc, int line, int level) const {
	if(!isgroup(line))
		return;
	color c1 = colors::text;
	auto isopen = list::isopen(line);
	int x = rc.x1 + rc.width() / 2;
	int y = rc.y1 + rc.height() / 2 - 1;
	if(ishilite(rc))
		current_hilite_treemark = line;
	circle(x, y, 6, c1);
	draw::line(x - 4, y, x + 4, y, c1);
	if(!isopen)
		draw::line(x, y - 4, x, y + 4, c1);
}

static void post_select() {
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
	return 1 + height / pixels_per_line;
}

void list::paint(const rect& rcorigin) {
	rect rc = rcorigin;
	pixels_per_width = rc.width() - 1;
	if(show_header)
		rc.y1 += rowheader(rc);
	rect rc1 = rc;
	rc.x1 += 1; rc.y1 += 1; // Padding for first string
	if(!pixels_per_line)
		pixels_per_line = getrowheight();
	lines_per_page = getlinesperpage(rc.height());
	correction();
	auto maximum = getmaximum();
	auto maximum_width = getmaximumwidth();
	if(!pixels_per_line)
		return;
	scroll scrollv(origin, lines_per_page, maximum, rc, false);
	scroll scrollh(origin_width, pixels_per_width, maximum_width, rc, true);
	scrollv.input();
	scrollh.input();
	control::paint(rc1);
	auto focused = isfocused(this);
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
	auto push_clip = clipping; setclip(rc);
	auto x1 = rc.x1 - origin_width, y1 = rc.y1;
	auto x2 = rc.x2;
	auto ix = origin;
	auto hl = colors::border.mix(colors::window, 12);
	while(true) {
		if(y1 >= rc.y2)
			break;
		if(ix >= maximum)
			break;
		rect rcm = {x1, y1, x2, y1 + pixels_per_line};
		if(hilite_odd_lines) {
			if(ix & 1)
				rectf({rcm.x1, rcm.y1, rcm.x2, rcm.y2}, hl);
		}
		if(show_grid_lines)
			line(rc.x1, rcm.y2 - 1, rc.x2, rcm.y2 - 1, colors::border);
		row(rcm, ix);
		y1 += pixels_per_line;
		ix++;
	}
	clipping = push_clip;
	scrollv.view(focused);
	scrollh.view(focused);
	if(drop_shadow) {
		rectf({rcorigin.x2 + 1, rcorigin.y1 + 4, rcorigin.x2 + 5, rcorigin.y2}, colors::black, 64);
		rectf({rcorigin.x1 + 4, rcorigin.y2, rcorigin.x2 + 5, rcorigin.y2 + 5}, colors::black, 64);
	}
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
				draw::execute(post_select, current - (lines_per_page - 1), getcolumn(), this);
			break;
		case KeyPageDown:
			if(current != (origin + lines_per_page - 1))
				draw::execute(post_select, origin + lines_per_page - 1, getcolumn(), this);
			else
				draw::execute(post_select, current + lines_per_page - 1, getcolumn(), this);
			break;
		case KeyEnter:
			post("Choose");
			break;
		case InputSymbol:
			if(hot.key >= 0x20) {
				auto time_clock = clock();
				if(!search_time || (time_clock - search_time) > 1500)
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