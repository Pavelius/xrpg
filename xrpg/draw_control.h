#include "crt.h"
#include "draw.h"
#include "handler.h"
#include "pointl.h"

#pragma once

#define ANREQ(c, f) {FO(c,f), sizeof(c::f)}
#define ANBIT(c, f, b) {FO(c,f), sizeof(c::f), b}

typedef bool(*fnvisible)(const void* object);

namespace metrics {
extern int					padding;
extern int					edit;
}
namespace draw {
enum class widthtype : unsigned char { Default, Resized, Fixed, Inner, Auto };
enum class selection : unsigned char { Cell, Text, Row };
enum class dock : unsigned char { Left, LeftBottom, Right, RightBottom, Bottom, Workspace };
enum class totaltype : unsigned char { None, Summarize, Maximum, Minimum, Average };
struct docki {
	const char*				id;
};
struct anyreq {
	unsigned short			offset;
	unsigned char			size;
	unsigned char			bit;
};
namespace controls {
class control {
	int						splitter;
public:
	constexpr control() : splitter(0) {}
	virtual ~control() {}
	void					contextmenu(const char** source);
	virtual void			execute(const char* id) {}
	virtual const char**	getcommands() const { return 0; }
	virtual const char**	getcommands(const char* parent) const { return 0; }
	virtual const sprite*	getimages() const { return std_images; }
	virtual const char*		getvalue(const char* id, stringbuilder& sb) const { return 0; }
	void					icon(int x, int y, const char* id, bool disabled) const;
	virtual bool			isallow(const char* id) const { return true; }
	virtual bool			isfocusable() const { return true; }
	bool					ishilited() const;
	virtual bool			ismodified() const { return false; }
	virtual void			paint(const rect& rc);
	void					post(const char* id) const;
	static const sprite*	std_images;
	virtual void			setvalue(const char* id, long value) {}
	int						toolbar(int x, int y, int width, int* next_x = 0) const;
	void					view(const rect& rc);
};
class scrollable : control {
	pointl					origin;
	pointl					maximum;
	point					wheels;
public:
	constexpr scrollable() : control(), origin{}, maximum{}, wheels{} {}
	rect					centerview(const rect& rc);
	virtual void			invalidate() { maximum.x = 0; }
	virtual void			redraw(const rect& rc) {}
	void					paint(const rect& rc) override;
	void					setmaximum(pointl v) { maximum = v; }
	void					setwheels(point v) { wheels = v; }
};
class list : public control {
	int						origin, origin_width, current, current_column;
	int						pixels_per_width, lines_per_page;
public:
	int						pixels_per_line;
	bool					show_header, show_selection, show_grid_lines, hilite_odd_lines, drop_shadow;
	constexpr list() : control(), origin(0), origin_width(0), current(0), current_column(0), pixels_per_width(0), lines_per_page(0), pixels_per_line(0),
		show_header(false), show_selection(true), show_grid_lines(true), hilite_odd_lines(true), drop_shadow(false) {}
	virtual void			cell(const rect& rc, int line, int column) const {}
	virtual void			collapse(int line) {}
	void					correction();
	void					ensurevisible();
	virtual void			expand(int line) {}
	void					expandall(int max_level);
	int						find(int line, int column, const char* value, int lenght = -1) const;
	int						getblockcount(int index) const;
	int						getcolumn() const { return current_column; }
	int						getcurrent() const { return current; }
	virtual int				getlevel(int line) const { return 0; }
	int						getline() const { return current; }
	int						getlinesperpage(int height) const;
	virtual const char*		getname(int line, int column, stringbuilder& sb) const { return 0; }
	int						getnextblock(int index, int increment) const;
	virtual int				getmaximum() const { return 0; }
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
	widthtype				size;
	totaltype				total;
	array*					source;
	anyreq					value;
	const visual*			method;
	explicit operator bool() const { return method != 0; }
	int						get(const void* object) const;
	const char*				get(const void* object, stringbuilder& sb) const;
	column&					set(widthtype v) { size = v; return *this; }
	//column&				set(column_s v) { flags.add(v); return *this; }
	column&					set(totaltype v) { total = v; return *this; }
	column&					set(array* v) { source = v; return *this; }
	column&					set(const anyreq& v) { value = v; return *this; }
	//column&				set(const fnlist& v) { plist = v; return *this; }
	column&					setwidth(int v) { width = v; return *this; }
};
struct table : control {
	column&					addcol(const char* id, const anyreq& req, const char* visual_id, array* source = 0);
	column&					addcol(const char* id, const char* visual_id);
	column&					addcolimage();
	virtual void*			addrow() = 0;
	void					cell(const rect& rc, int line, int column) const {}
	void					cellbox(const table* p, const rect& rc, int line, int column);
	void					celldate(const table* p, const rect& rc, int line, int column);
	void					celldatetime(const table* p, const rect& rc, int line, int column);
	void					cellimage(const table* p, const rect& rc, int line, int column);
	void					cellimagest(const table* p, const rect& rc, int line, int column);
	void					cellrownumber(const table* p, const rect& rc, int line, int column);
	void					cellhilite(const table* p, const rect& rc, int line, int columen, const char* text, image_flag_s aling) const;
	void					cellnumber(const table* p, const rect& rc, int line, int column);
	void					cellpercent(const table* p, const rect& rc, int line, int column);
	void					celltext(const table* p, const rect& rc, int line, int column);
};
struct visual {
	typedef void (*fnrender)(const table* p, const rect& rc, int line, int column);
	const char*				id;
	//image_flag_s			align;
	int						minimal_width, default_width;
	widthtype				size;
	totaltype				total;
	fnrender				render;
	fnrender				change;
	//table::fncompare		comparer;
	bool					change_one_click;
	//fnlist				plist;
	explicit operator bool() const { return render != 0; }
	//static const visual*	find(const char* id);
};
}
}