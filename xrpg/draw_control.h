#include "anyreq.h"
#include "handler.h"
#include "markup.h"
#include "point.h"

#pragma once

struct sprite;
typedef void (*fnevent)();

namespace draw {
enum class widtht : unsigned char { Default, Resized, Fixed, Inner, Auto };
enum class selection : unsigned char { Cell, Text, Row };
enum class dock : unsigned char { Left, LeftBottom, Right, RightBottom, Bottom, Workspace };
enum class totalt : unsigned char { None, Summarize, Maximum, Minimum, Average };
enum class columnf : unsigned char { ReadOnly, Visible };
namespace controls {
class control {
public:
	struct plugin {
		struct builder {
			virtual bool	canopen(const char* url) const;
			virtual control* create(const char* url) = 0;
			virtual void	destroy(control* v) {}
			virtual void	getextensions(stringbuilder& sb) const {}
			virtual bool	read(const char* url) { return false; }
		};
		const char*			id;
		dock				position;
		bool				visible;
		plugin*				next;
		static plugin*		first;
		plugin(const char* id, dock position);
		static const plugin* find(const char* id);
		virtual builder*	getbuilder() { return 0; }
		virtual control*	getcontrol() = 0;
	};
	rect					client;
	constexpr control() : client() {}
	virtual ~control() {}
	virtual void			activating() {}
	void					contextmenu(const char** source);
	virtual void			deactivating() {}
	bool					dropdown(const rect& rc);
	virtual bool			execute(const char* id, bool run) { return false; }
	virtual const char**	getcommands() const { return 0; }
	virtual const char**	getcommands(const char* parent) const { return 0; }
	virtual const sprite*	getimages() const { return std_images; }
	virtual const char*		getvalue(const char* id, stringbuilder& sb) const { return 0; }
	virtual bool			hastoolbar() const { return true; }
	void					icon(int x, int y, const char* id, bool disabled) const;
	virtual bool			isfocusable() const { return true; }
	bool					ishilited() const;
	virtual bool			ismodified() const { return false; }
	virtual void			paint(const rect& rc);
	void					post(const char* id) const;
	virtual void			read(serializer& e) {}
	virtual void			save(const char* url) const {}
	static const sprite*	std_images;
	virtual void			setvalue(const char* id, long value) {}
	int						toolbar(int x, int y, int width, int* next_x = 0) const;
	virtual void			write(serializer& e) const {}
	void					view(const rect& rc, bool show_border, bool show_background);
};
class scrollable : control {
	int						origin_x, origin_y;
	int						maximum_x, maximum_y;
	int						wheel_x, wheel_y;
public:
	constexpr scrollable() : control(), origin_x(), origin_y(), maximum_x(), maximum_y(), wheel_x(), wheel_y() {}
	rect					centerview(const rect& rc);
	virtual void			invalidate() { maximum_x = 0; }
	virtual void			redraw(const rect& rc) {}
	void					paint(const rect& rc) override;
	void					setmaximum(int x, int y) { maximum_x = x; maximum_y = y; }
	void					setwheels(int x, int y) { wheel_x = x; wheel_y = y; }
};
class list : public control {
protected:
	int						origin, origin_x;
	int						page, page_x;
	int						current, current_column;
	static void				post_select();
public:
	int						pixels_per_line;
	bool					show_header, show_selection, show_grid_lines, hilite_odd_lines;
	static int				current_hilite_treemark, current_hilite_row, current_hilite_column;
	constexpr list(bool show_header = false) : control(), origin(0), origin_x(0), page(0), page_x(0),
		current(0), current_column(0), pixels_per_line(0),
		show_header(show_header), show_selection(true), show_grid_lines(false), hilite_odd_lines(true) {}
	virtual void			collapse(int line) {}
	void					correction();
	virtual void			ensurevisible();
	virtual void			expand(int line) {}
	void					expandall(int max_level);
	int						find(int line, int column, const char* value, int lenght = -1) const;
	int						getblockcount(int index) const;
	int						getcolumn() const { return current_column; }
	int						getcurrent() const { return current; }
	virtual int				getident() const { return 0; }
	virtual int				getlevel(int line) const { return 0; }
	int						getline() const { return current; }
	int						getlinesperpage(int height) const;
	virtual const char*		getname(int line, int column, stringbuilder& sb) const { return 0; }
	int						getnextblock(int index, int increment) const;
	virtual int				getmaximum() const = 0;
	virtual int				getmaximumwidth() const { return 0; }
	int						getparent(int index) const;
	int						getroot(int index) const;
	virtual int				getrowimage(int line) const { return -1; }
	static int				getrowheight();
	virtual const sprite*	gettreeimages() const { return std_tree_images; }
	void					hilight(const rect& rc) const;
	virtual bool			isgroup(int line) const { return false; }
	bool					isopen(int index) const;
	virtual void			mouseclick() const;
	virtual void			mousehiliting(const rect& rc, point mouse) const;
	void					paint(const rect& rc) override;
	void					paintrows(const rect& rc);
	virtual void			row(const rect& rc, int line) const;
	virtual int				rowheader(const rect& rc) const { return 0; }
	virtual void			rowhilite(const rect& rc, int index) const;
	virtual void			select(int index, int column);
	static const sprite*	std_tree_images;
	void					toggle(int index);
	void					treemark(const rect& rc, int line, int level) const;
};
struct visual;
struct column {
	const char*				id;
	short					width;
	widtht					size;
	totalt					total;
	array*					source;
	anyreq					value;
	const visual*			method;
	unsigned				flags;
	unsigned				align;
	fnlist					plist;
	explicit operator bool() const { return method != 0; }
	int						get(const void* object) const;
	const char*				get(const void* object, stringbuilder& sb) const;
	bool					is(columnf v) const { return flags & (1 << static_cast<int>(v)); }
	column&					set(widtht v) { size = v; return *this; }
	column&					set(columnf v) { flags |= 1 << static_cast<int>(v); return *this; }
	column&					set(totalt v) { total = v; return *this; }
	column&					set(array* v) { source = v; return *this; }
	column&					set(const anyreq& v) { value = v; return *this; }
	void					set(const void* object, int v) const;
	column&					setwidth(int v) { width = v; return *this; }
};
class table : public list {
	array&					rows;
	int						maximum_x;
	column&					addcolimp(const char* name, const anyreq* req, const char* visual_id, array* source);
	int						getvalid(int column, int direction) const;
	void					update_columns(const rect& rc);
public:
	typedef int				(table::*fncompare)(int i1, int i2, int column) const;
	struct sortparam {
		int					column;
		int					multiplier;
	};
	vector<column>			columns;
	bool					no_change_order, no_change_count, read_only, show_totals;
	selection				select_mode;
	static const visual		visuals[];
	constexpr table(array& rows, selection mode = selection::Cell) : list(true), rows(rows), maximum_x(), columns(),
		no_change_order(false), no_change_count(false), read_only(false), show_totals(false),
		select_mode(mode) {}
	column&					addcol(const char* id, const anyreq& req, const char* visual_id, array* source = 0);
	column&					addcol(const char* id, const char* visual_id);
	column&					addcolimage();
	virtual void*			addrow() { return rows.add(); }
	void					cell(const rect& rc, int line, int column, const char* label);
	void					cellbox(const rect& rc, int line, int column);
	void					celldate(const rect& rc, int line, int column);
	void					celldatetime(const rect& rc, int line, int column);
	void					cellimage(const rect& rc, int line, int column);
	void					cellimagest(const rect& rc, int line, int column);
	void					cellrownumber(const rect& rc, int line, int column);
	void					cellhilite(const rect& rc, int line, int columen, const char* text, unsigned aling) const;
	void					cellnumber(const rect& rc, int line, int column);
	void					cellpercent(const rect& rc, int line, int column);
	void					celltext(const rect& rc, int line, int column);
	void					changecheck(const rect& rc, int line, int column);
	void					changenumber(const rect& rc, int line, int column);
	void					changeref(const rect& rc, int line, int column);
	void					changetext(const rect& rc, int line, int column);
	void					clickcolumn(int column) const;
	int						comparest(int i1, int i2, int column) const;
	int						comparenm(int i1, int i2, int column) const;
	int						comparer(int i1, int i2, const sortparam* param, int count) const;
	void					ensurevisible() override;
	bool					execute(const char* id, bool run) override;
	virtual void*			get(int line) const { return rows.ptr(line); }
	const char**			getcommands() const override;
	void*					getcurrent() const { return (current >= getmaximum()) ? 0 : get(current); }
	const char*				getheader(int column, stringbuilder& sb) const;
	int						getmaximum() const override { return rows.getcount(); }
	rect					getrect(int line, int column) const;
	int						gettotal(int column) const;
	int						getvalid(int column) const { return column; }
	virtual const visual**	getvisuals() const;
	void					mouseclick() const;
	void					paint(const rect& rc) override;
	void					row(const rect& rc, int index) const override;
	int						rowheader(const rect& rc) const override;
	void					rowtotal(const rect& rc) const;
	void					select(int index, int column) override;
	void					sort(int column, bool ascending);
	void					sort(int i1, int i2, sortparam* ps, int count);
	void					write(serializer& file) const override;
	bool					write(const char* url, bool include_header) const;
};
class tableref : public table {
	array					rows;
public:
	constexpr tableref() : table(rows), rows(sizeof(void*)) {}
	virtual void			addref(void* v) { rows.add(&v); }
	virtual void*			get(int line) const override { return *((void**)rows.ptr(line)); }
};
struct visual {
	typedef void (table::*fnrender)(const rect& rc, int line, int column);
	const char*				id;
	unsigned				align;
	int						minimal_width, default_width;
	widtht					size;
	totalt					total;
	fnrender				render;
	fnrender				change;
	table::fncompare		comparer;
	bool					change_one_click;
	fnlist					plist;
	explicit operator bool() const { return render != 0; }
	const visual*			find(const char* id) const;
};
}
void						activate(controls::control* p);
void						close(controls::control* p);
bool						edit(controls::control& e, fnevent heartbeat);
const char*					getlabel(const void* object, stringbuilder& sb);
controls::control*			openurl(const char* url);
}
