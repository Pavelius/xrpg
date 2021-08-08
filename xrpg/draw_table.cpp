#include "command.h"
#include "datetime.h"
#include "draw.h"
#include "draw_button.h"
#include "draw_control.h"
#include "draw_focus.h"
#include "io_plugin.h"

using namespace draw;
using namespace draw::controls;

static int		current_sort_column;
static bool		current_order;

static void table_sort_column() {
	auto p = (table*)hot.object;
	if(hot.param == current_sort_column)
		current_order = !current_order;
	else {
		current_sort_column = hot.param;
		current_order = true;
	}
	p->sort(hot.param, current_order);
}

int table::comparest(int i1, int i2, int column) const {
	char t1[260]; stringbuilder sb1(t1);
	char t2[260]; stringbuilder sb2(t2);
	auto n1 = columns[column].get(get(i1), sb1);
	auto n2 = columns[column].get(get(i2), sb2);
	return strcmp(n1, n2);
}

int table::comparenm(int i1, int i2, int column) const {
	auto pc = &columns[column];
	return pc->get(get(i1)) - pc->get(get(i2));
}

int table::comparer(int i1, int i2, const sortparam* param, int count) const {
	for(auto i = 0; i < count; i++) {
		auto pc = columns[param[i].column].method->comparer;
		if(!pc)
			continue;
		auto r = (this->*pc)(i1, i2, param[i].column);
		if(r == 0)
			continue;
		return r * param[i].multiplier;
	}
	return 0;
}

void table::sort(int i1, int i2, sortparam* ps, int count) {
	if(i2 <= i1)
		return;
	for(int i = i2; i > i1; i--) {
		for(int j = i1; j < i; j++)
			if(comparer(j, j + 1, ps, count) > 0)
				rows.swap(j, j + 1);
	}
}

void table::sort(int column, bool ascending) {
	sortparam e;
	e.column = column;
	e.multiplier = ascending ? 1 : -1;
	sort(0, getmaximum() - 1, &e, 1);
}

void table::clickcolumn(int column) const {
	draw::execute(table_sort_column, column, 0, this);
}

int	column::get(const void* object) const {
	return value.get(value.ptr(object));
}

const char* column::get(const void* object, stringbuilder& sb) const {
	if(plist.getname) {
		if(source)
			return plist.getname(source->ptr(value.get(value.ptr(object))), sb);
		return plist.getname(object, sb);
	} else if(source)
		return getpresent(value.ptr(object), value.size, true, source, sb);
	else if(value.size == sizeof(const char*))
		return value.gets(value.ptr(object));
	return "";
}

void column::set(const void* object, int v) const {
	//path.set(path.ptr(object), v);
}

void table::update_columns(const rect& rc) {
	int w1 = rc.width();
	int w2 = 0;
	int c2 = 0;
	const int min_width = 8;
	if(!columns)
		return;
	// Get count of al elements
	for(auto& e : columns) {
		if(e.size == widtht::Auto) {
			w2 += min_width;
			c2++;
		} else {
			if(e.method->minimal_width) {
				if(e.width < e.method->minimal_width)
					e.width = e.method->minimal_width;
			}
			w2 += e.width;
		}
	}
	// Calculate auto sized width
	if(w2 < w1 && c2) {
		int d1 = w1 - w2;
		int d2 = d1 / c2;
		for(auto& e : columns) {
			if(e.size == widtht::Auto) {
				if(d2 < d1)
					e.width = min_width + d2;
				else {
					e.width = min_width + d1;
					break;
				}
			}
		}
	}
}

int	table::getvalid(int column, int direction) const {
	unsigned count = 0;
	while(true) {
		if(count >= columns.count)
			return getcolumn();
		if(column < 0 || (unsigned)column >= columns.count)
			return getcolumn();
		if(!columns[column].is(columnf::Visible)
			|| columns[column].size == widtht::Inner) {
			if(direction)
				column += direction;
			else
				column += 1;
			count++;
			continue;
		}
		break;
	}
	return column;
}

void table::select(int index, int column) {
	current_column = column;
	list::select(index, column);
}

static void proc_mouseselect() {
	auto p = (table*)draw::hot.object;
	auto i = p->getvalid(list::current_hilite_column);
	p->select(list::current_hilite_row, i);
	if(p->columns[i].method->change_one_click)
		p->execute("Change", true);
}

void table::mouseclick() const {
	if(hot.key == MouseLeft && hot.pressed)
		draw::execute(proc_mouseselect, 0, 0, this);
}

const char* table::getheader(int column, stringbuilder& sb) const {
	return getnm(columns[column].id);
}

int table::rowheader(const rect& rc) const {
	static int base_width;
	char temp[260]; auto height = getrowheight();
	rect rch = {rc.x1, rc.y1, rc.x2, rc.y1 + height};
	color b1 = colors::button.lighten();
	color b2 = colors::button.darken();
	if(no_change_order)
		b1 = b1.mix(b2, 192);
	gradv(rch, b1, b2);
	rectb(rch, colors::border);
	auto push_clip = clipping; setclip({rc.x1, rc.y1, rc.x2, rc.y2 + 1});
	color active = colors::button.mix(colors::button, 128);
	color a1 = active.lighten();
	color a2 = active.darken();
	rect r1;
	r1.x1 = rch.x1 - origin_x;
	r1.x2 = r1.x1; r1.x2++;
	r1.y1 = rch.y1;
	r1.y2 = rch.y2;
	for(unsigned i = 0; i < columns.count; i++) {
		if(!columns[i].is(columnf::Visible))
			continue;
		if(dragactive(&columns[i])) {
			hot.cursor = cursor::LeftRight;
			auto new_width = base_width + (hot.mouse.x - dragmouse.x);
			auto pw = columns[i].method;
			if(new_width < pw->minimal_width)
				new_width = pw->minimal_width;
			const_cast<column&>(columns[i]).width = new_width;
		}
		r1.x2 = r1.x2 + columns[i].width;
		if(r1.x1 == (rch.x1 - origin_x))
			r1.x1++;
		if(columns[i].size == widtht::Inner)
			continue;
		auto a = ishilite(r1);
		if(!no_change_order) {
			if(a) {
				if(hot.pressed)
					gradv({r1.x1, r1.y1 + 1, r1.x2, r1.y2}, a2, a1);
				else
					gradv({r1.x1, r1.y1 + 1, r1.x2, r1.y2}, a1, a2);
				if(hot.key == MouseLeft && !hot.pressed)
					clickcolumn(i);
			}
		}
		line(r1.x2 - 1, r1.y1, r1.x2 - 1, r1.y2, colors::border);
		stringbuilder sb(temp); temp[0] = 0;
		auto p = getheader(i, sb);
		if(p)
			textc(r1.x1 + metrics::edit, r1.y1 + metrics::edit, r1.width() - metrics::edit * 2, p);
		a = ishilite({r1.x2 + metrics::edit - 1, r1.y1, r1.x2 + metrics::edit + 1, r1.y2});
		if(a) {
			hot.cursor = cursor::LeftRight;
			if(hot.pressed && hot.key == MouseLeft) {
				dragbegin(&columns[i]);
				base_width = columns[i].width;
				dragmouse.x = hot.mouse.x;
			}
		}
		r1.x1 = r1.x2;
	}
	clipping = push_clip;
	return height;
}

int	table::gettotal(int column) const {
	auto& c = columns[column];
	auto type = c.total;
	if(type == totalt::None)
		return 0;
	auto m = getmaximum();
	auto result = 0;
	switch(type) {
	case totalt::Maximum:
		for(auto i = 0; i < m; i++) {
			auto v = c.get(get(i));
			if(v > result)
				result = v;
		}
		break;
	case totalt::Minimum:
		result = 0x7FFFFFFF;
		for(auto i = 0; i < m; i++) {
			auto v = c.get(get(i));
			if(v < result)
				result = v;
		}
		break;
	default:
		for(auto i = 0; i < m; i++)
			result += c.get(get(i));
		if(type == totalt::Average)
			result = result / m;
		break;
	}
	return result;
}

void table::rowtotal(const rect& rc) const {
	char temp[260]; auto height = getrowheight();
	rect rch = {rc.x1, rc.y1, rc.x2, rc.y1 + height};
	color b1 = colors::button.lighten();
	color b2 = colors::button.darken();
	gradv(rch, b1, b2);
	rectb(rch, colors::border);
	auto push_clip = clipping; setclip({rc.x1, rc.y1, rc.x2, rc.y2 + 1});
	rect r1;
	r1.x1 = rch.x1 - origin_x;
	r1.x2 = r1.x1; r1.x2++;
	r1.y1 = rch.y1;
	r1.y2 = rch.y2;
	for(unsigned i = 0; i < columns.count; i++) {
		if(!columns[i].is(columnf::Visible))
			continue;
		r1.x2 = r1.x2 + columns[i].width;
		if(columns[i].size == widtht::Inner)
			continue;
		line(r1.x2, r1.y1, r1.x2, r1.y2, colors::border);
		auto result = gettotal(i);
		temp[0] = 0;
		if(result) {
			stringbuilder sb(temp);
			sb.add("%1i", result);
			auto r2 = r1 + metrics::edit;
			draw::text(r2, temp, AlignRight | TextSingleLine);
		}
		r1.x1 = r1.x2;
	}
	clipping = push_clip;
}

void table::row(const rect& rc, int index) const {
	ishilite(rc);
	if(select_mode == selection::Row)
		rowhilite(rc, index);
	auto x1 = rc.x1;
	auto need_tree = true;
	auto level_ident = 0;
	for(unsigned i = 0; i < columns.count; i++) {
		auto pc = columns.ptr(i);
		if(!pc->is(columnf::Visible))
			continue;
		if(need_tree) {
			auto level = getlevel(index);
			if(level) {
				int dy = getident();
				level_ident = level * dy;
				x1 = x1 + level_ident;
				treemark({x1 - dy, rc.y1, x1, rc.y2}, index, level);
			}
			need_tree = false;
		}
		rect r1 = {x1, rc.y1, x1 + pc->width - 1, rc.y2 - 1};
		if(level_ident) {
			if(columns[i].size != widtht::Inner && columns[i].size != widtht::Fixed) {
				auto mx = r1.width() - metrics::edit * 2;
				if(mx > level_ident)
					mx = level_ident;
				r1.x2 -= mx;
				level_ident -= mx;
			}
		}
		if(show_grid_lines && columns[i].size != widtht::Inner)
			draw::line(r1.x2, r1.y1, r1.x2, r1.y2, colors::border);
		ishilite(r1);
		((const_cast<table*>(this))->*pc->method->render)(r1, index, i);
		x1 += pc->width;
	}
}

void table::ensurevisible() {
	list::ensurevisible();
	auto rc = getrect(current, current_column);
	auto x1 = rc.x1 - (client.x1 - origin_x);
	if(rc.x2 > client.x2)
		origin_x = origin_x + rc.width();
	if(rc.x1 < client.x1)
		origin_x = x1;
}

rect table::getrect(int row, int column) const {
	rect rs;
	rs.x1 = client.x1 - origin_x + 1;
	rs.x2 = rs.x1;
	rs.y1 = client.y1 + pixels_per_line * (row - origin) + 1;
	rs.y2 = rs.y1 + pixels_per_line;
	if(column >= (int)columns.getcount())
		column = columns.getcount() - 1;
	for(auto i = 0; i <= column; i++) {
		if(!columns[i].is(columnf::Visible))
			continue;
		rs.x1 = rs.x2;
		rs.x2 = rs.x1 + columns[i].width;
	}
	return rs;
}

void table::paint(const rect& rc) {
	rect rt;
	rt.y1 = rc.y1;
	rt.y2 = rc.y2;
	update_columns(rc);
	maximum_x = 0;
	for(unsigned i = 0; i < columns.count; i++) {
		if(!columns[i].is(columnf::Visible))
			continue;
		rt.x1 = rc.x1 - origin_x + maximum_x;
		rt.x2 = rt.x1 + columns[i].width;
		if(hot.mouse.in(rt))
			current_hilite_column = i;
		maximum_x += columns[i].width;
		//current_column_maximum = i;
	}
	// Don't show most right column border
	if(maximum_x > 0)
		maximum_x -= 1;
	if(show_totals) {
		list::paint({rc.x1, rc.y1, rc.x2, rc.y2 - getrowheight()});
		rowtotal({rc.x1, rc.y2 - getrowheight(), rc.x2, rc.y2});
	} else
		list::paint(rc);
	if(isfocused(this)) {
		int v;
		switch(hot.key) {
		case KeyHome:
			v = getvalid(0, 1);
			if(v != current_column)
				draw::execute(post_select, getline(), v, this);
			break;
		case KeyEnd:
			v = getvalid(columns.getcount() - 1, 1);
			if(v != current_column)
				draw::execute(post_select, getline(), v, this);
			break;
		case KeyLeft:
			v = getvalid(current_column - 1, -1);
			if(v != current_column)
				draw::execute(post_select, getline(), v, this);
			break;
		case KeyRight:
			v = getvalid(current_column + 1);
			if(v != current_column && v < (int)columns.getcount())
				draw::execute(post_select, getline(), v, this);
			break;
		}
	}
}

const visual* visual::find(const char* id) const {
	for(auto p = this; *p; p++) {
		if(strcmp(p->id, id) == 0)
			return p;
	}
	return 0;
}

column&	table::addcolimp(const char* name, const anyreq* req, const char* visual_id, array* source) {
	const visual* pf = 0;
	auto p = columns.add();
	memset(p, 0, sizeof(column));
	p->id = name;
	if(visual_id) {
		for(auto pp = getvisuals(); pp && *pp; pp++) {
			pf = (*pp)->find(visual_id);
			if(pf)
				break;
		}
	}
	if(pf) {
		p->method = pf;
		p->set(columnf::Visible);
	} else
		p->method = visuals;
	if(req)
		p->value = *req;
	auto text_width = draw::textw('0');
	if(!text_width)
		text_width = 7;
	p->plist = p->method->plist;
	p->size = p->method->size;
	p->width = p->method->default_width;
	if(p->width < 0)
		p->width = -p->width * text_width + 4;
	p->total = p->method->total;
	p->align = p->method->align;
	p->source = source;
	if(!p->width)
		p->width = 100;
	current_column = getvalid(current_column, 1);
	return *p;
}

column&	table::addcol(const char* name, const anyreq& req, const char* visual_id, array* source) {
	return addcolimp(name, &req, visual_id, source);
}

column&	table::addcol(const char* name, const char* visual_id) {
	return addcolimp(name, 0, visual_id, 0);
}

column& table::addcolimage() {
	return addcol(0, "standart_image");
}

static bool changefield(rect rc, void* row, const column& col, stringbuilder& sb, bool isnumber) {
	rc.offset(metrics::edit);
	draw::rectf({rc.x1 - 1, rc.y1 - 1, rc.x2, rc.y2}, colors::window);
	rc.x2 -= 1;
	return draw::edit(rc, col.value.ptr(row), col.value.size, col.align|TextSingleLine, isnumber);
}

void table::changenumber(const rect& rc, int line, int column) {
	auto p = get(line);
	char temp[32]; stringbuilder sb(temp);
	sb.add("%1i", columns[column].get(get(line)));
	if(changefield(rc, p, columns[column], sb, true)) {
		long value = 0;
		stringbuilder::read(temp, value);
		columns[column].set(get(line), value);
	}
}

void table::changetext(const rect& rc, int line, int column) {
	auto p = get(line);
	char temp[8192]; stringbuilder sb(temp);
	auto value = (const char*)columns[column].get(get(line));
	if(!value)
		temp[0] = 0;
	else
		sb.add(value);
	if(changefield(rc, p, columns[column], sb, false))
		columns[column].set(get(line), (size_t)szdup(temp));
}

void table::changeref(const rect& rc, int line, int column) {
	if(!columns[column].source)
		return;
	auto p = get(line);
	auto& col = columns[column];
	//const anyval av((char*)p + columns[column].path.offset, columns[column].path.size, 0);
	fielcd(rc, col.value.ptr(p), col.value.size, *col.source, p, col.plist, true);
}

void table::changecheck(const rect& rc, int line, int column) {
	auto p = get(line);
	auto v = columns[column].get(p);
	auto b = 1 << columns[column].value.bit;
	if((v & b) != 0)
		columns[column].set(p, v & (~b));
	else
		columns[column].set(p, v | b);
}

void table::cell(const rect& rc, int line, int column, const char* label) {
	if(!label)
		return;
	auto align = columns[column].align;
	cellhilite(rc, line, column, label, align);
	bool clipped = false;
	rect r1 = rc + metrics::edit;
	textc(r1.x1, r1.y1, r1.width(), label, -1, align, &clipped);
	if(clipped) {
		if(ishilite(r1))
			tooltips(r1.x1, r1.y1, 200, label);
	}
}

void table::cellhilite(const rect& rc, int line, int column, const char* text, unsigned aling) const {
	if(line == current && column == current_column) {
		switch(select_mode) {
		case selection::Cell:
			hilight(rc);
			break;
		case selection::Text:
			if(!text)
				break;
			switch(aling) {
			case AlignRight:
			case AlignRightCenter:
			case AlignRightBottom:
				hilight({rc.x2 + metrics::edit - (draw::textw(text) + 2), rc.y1, rc.x2, rc.y2});
				break;
			default:
				hilight({rc.x1, rc.y1, rc.x1 + (draw::textw(text) + 2) + metrics::edit * 2, rc.y2});
				break;
			}
			break;
		default:
			break;
		}
	}
}

void table::celltext(const rect& rc, int line, int column) {
	char temp[260]; stringbuilder sb(temp);
	auto ps = columns[column].get(get(line), sb);
	cell(rc, line, column, ps);
}

void table::cellimagest(const rect& rc, int line, int column) {
	auto s = gettreeimages();
	if(!s)
		return;
	auto v = getrowimage(line);
	if(v == -1)
		return;
	image(rc.x1 + rc.width() / 2, rc.y1 + rc.height() / 2, s, v, 0);
}

void table::cellimage(const rect& rc, int line, int column) {
	auto s = gettreeimages();
	if(!s)
		return;
	auto v = columns[column].get(get(line));
	image(rc.x1 + rc.width() / 2, rc.y1 + rc.height() / 2, s, v, 0);
}

void table::cellrownumber(const rect& rc, int line, int column) {
	char temp[32]; stringbuilder sb(temp);
	sb.add("%1i", line + 1);
	cell(rc, line, column, temp);
}

void table::cellnumber(const rect& rc, int line, int column) {
	char temp[32]; stringbuilder sb(temp);
	sb.add("%1i", columns[column].get(get(line)));
	cell(rc, line, column, temp);
}

void table::cellpercent(const rect& rc, int line, int column) {
	char temp[32]; stringbuilder sb(temp);
	sb.add("%1i%%", columns[column].get(get(line)));
	cell(rc, line, column, temp);
}

void table::celldate(const rect& rc, int line, int column) {
	datetime d(columns[column].get(get(line)));
	if(!d)
		return;
	char temp[260]; stringbuilder sb(temp);
	sb.add("%1.2i.%2.2i.%3.4i", d.day(), d.month(), d.year());
	cell(rc, line, column, temp);
}

void table::celldatetime(const rect& rc, int line, int column) {
	datetime d(columns[column].get(get(line)));
	if(!d)
		return;
	char temp[260]; stringbuilder sb(temp);
	sb.add("%1.2i.%2.2i.%3.2i %4.2i:%5.2i",
		d.day(), d.month(), d.year(), d.hour(), d.minute());
	cell(rc, line, column, temp);
}

void table::cellbox(const rect& rc, int line, int column) {
	//unsigned flags = 0;
	//auto v = columns[column].get(get(line));
	//auto b = 1 << columns[column].value.bit;
	//if(v & b)
	//	flags |= Checked;
	cellhilite(rc, line, column, 0, AlignCenter);
	//clipart(rc.x1 + 2, rc.y1 + imax((rc.height() - 14) / 2, 0), 0, flags, ":check");
}

const visual** table::getvisuals() const {
	static const visual* elements[] = {visuals, 0};
	return elements;
}

bool table::execute(const char* id, bool run) {
	if(equal(id, "SortAs")) {
		if(no_change_order)
			return false;
		if(getmaximum() <= 1)
			return false;
		if(run)
			sort(current_column, true);
	} else if(equal(id, "SortDs")) {
		if(no_change_order)
			return false;
		if(getmaximum() <= 1)
			return false;
		if(run)
			sort(current_column, false);
	} else if(equal(id, "MoveUp")) {
		if(no_change_order)
			return false;
		if(current <= 0)
			return false;
		if(getmaximum() == 1)
			return false;
		if(run) {
			rows.swap(current - 1, current);
			select(current - 1, getcolumn());
		}
	} else if(equal(id, "MoveDown")) {
		if(no_change_order)
			return false;
		if(current >= int(getmaximum() - 1))
			return false;
		if(run) {
			rows.swap(current + 1, current);
			select(current + 1, getcolumn());
		}
	} else if(equal(id, "Change") || equal(id, "Choose")) {
		if(read_only)
			return false;
		if(!columns)
			return false;
		if(current >= getmaximum() || current_column == -1)
			return false;
		auto& col = columns[current_column];
		if(col.is(columnf::ReadOnly))
			return false;
		auto pv = col.method;
		if(!pv || !pv->change)
			return false;
		auto rc = getrect(current, current_column);
		if(!rc)
			return false;
		if(run)
			(this->*pv->change)(rc, current, current_column);
	} else if(equal(id, "Add")) {
		if(no_change_count)
			return false;
		if(run) {
			auto n = getmaximum();
			auto p = addrow();
			if(p) {
				if(getmaximum())
					select(n, getvalid(0));
				post("Change");
				setneedupdate();
			}
		}
	} else if(equal(id, "AddCopy")) {
		if(no_change_count)
			return false;
		if(run) {
			auto p = addrow();
			if(p) {
				auto pc = get(current);
				memcpy(p, pc, rows.getsize());
				if(getmaximum())
					select(rows.indexof(p), getvalid(0));
				post("Change");
				setneedupdate();
			}
		}
	} else if(equal(id, "Remove")) {
		if(read_only)
			return false;
		if(no_change_count)
			return false;
		if(!getmaximum())
			return false;
		if(run)
			rows.remove(current);
		return true;
	} else if(equal(id, "SaveAs")) {
		static char url[261];
		if(!getmaximum())
			return false;
		if(run) {
			char filter[1024]; stringbuilder sb(filter);
			io::plugin::getfilter(sb);
			if(!dialog::save(getnm("ExportTableData"), url, filter, 0))
				return false;
			write(url, false);
		}
	}
	return true;
}

void table::write(serializer& file) const {
	if(!columns)
		return;
	file.open("columns");
	for(auto& e : columns) {
		file.open("column");
		file.set("id", e.id);
		file.set("width", e.width);
		//file.set("size", e.size);
		//file.set("total", e.total);
		file.close("column");
	}
	file.close("columns");
}

bool table::write(const char* url, bool include_header) const {
	auto pp = io::plugin::find(szext(url));
	if(!pp)
		return false;
	io::file file(url, StreamWrite | StreamText);
	if(!file)
		return false;
	auto pw = pp->write(file);
	if(!pw)
		return false;
	pw->open("Table");
	if(include_header)
		write(*pw);
	pw->open("Rows", serializer::kind::Array);
	for(auto i = 0; i < getmaximum(); i++) {
		auto p = get(i);
		pw->open("Row");
		for(auto& e : columns) {
			if(e.method->render == &table::cellnumber
				|| e.method->render == &table::cellrownumber
				|| e.method->render == &table::cellpercent
				|| e.method->render == &table::celldate
				|| e.method->render == &table::celldatetime) {
				pw->set(e.id, e.get(p));
			} else {
				char temp[4096]; stringbuilder sb(temp);
				pw->set(e.id, e.get(p, sb));
			}
		}
		pw->close("Row");
	}
	pw->close("Rows", serializer::kind::Array);
	pw->close("Table");
	delete pw;
	return true;
}

const char** table::getcommands() const {
	static const char* commands[] = {"Add", "AddCopy", "Change", "Remove", 0};
	return commands;
}

const visual table::visuals[] = {
	{"Number", AlignRight, 8, 80, widtht::Resized, totalt::Summarize, &table::cellnumber, &table::changenumber, &table::comparenm},
	{"RowNumber", AlignCenter, 8, 40, widtht::Resized, totalt::None, &table::cellrownumber},
	{"Checkbox", AlignCenter, 28, 28, widtht::Fixed, totalt::None, &table::cellbox, &table::changecheck, 0, true},
	{"Date", AlignLeft, 8, -12, widtht::Resized, totalt::None, &table::celldate, 0, &table::comparenm},
	{"DateTime", AlignLeft, 8, -16, widtht::Resized, totalt::None, &table::celldatetime, 0, &table::comparenm},
	{"Text", AlignLeft, 8, 200, widtht::Resized, totalt::None, &table::celltext, &table::changetext, &table::comparest},
	{"Enum", AlignLeft, 8, 200, widtht::Resized, totalt::None, &table::celltext, &table::changeref, 0, false, {getenumname}},
	{"Percent", AlignRight, 40, 60, widtht::Resized, totalt::Average, &table::cellpercent, &table::changenumber},
	{"Image", AlignCenter, 20, 20, widtht::Inner, totalt::None, &table::cellimage, 0, &table::comparenm},
	{"StandartImage", AlignCenter, 20, 20, widtht::Inner, totalt::None, &table::cellimagest, 0, 0},
	{}};