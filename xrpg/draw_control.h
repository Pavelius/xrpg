#include "draw.h"
#include "markup.h"
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
enum class widthtype : unsigned char { Default, Resized, Fixed, Inner, Auto, };
enum class selection : unsigned char { Cell, Text, Row, };
enum class dock : unsigned char { Left, LeftBottom, Right, RightBottom, Bottom, Workspace };
enum class totaltype : unsigned char { None, Summarize, Maximum, Minimum, Average };
struct docki {
	const char*				id;
};
namespace controls {
struct visual;
class control;
class builder {
	void					render(const control* parent, const char** commands, bool& separator, int& count);
public:
	builder() = default;
	virtual ~builder() {}
	virtual void			add(const control* parent, const char* id) = 0;
	virtual void			addseparator() = 0;
	virtual const char*		finish() const { return 0; }
	void					render(const control* parent, const char** commands);
	virtual void			start() const {}
};
class control {
	int						splitter;
public:
	constexpr control() : splitter(0) {}
	virtual ~control() {}
	void					contextmenu(const char** source);
	void					contextmenu(const char** source, builder& builder);
	static const char*		commands_edit[];
	void					icon(int x, int y, const char* id, bool disabled) const;
	virtual void			execute(const char* id) {}
	virtual const char**	getcommands() const { return 0; }
	virtual const char**	getcommands(const char* parent) const;
	virtual sprite*			getimages() const { return std_images; }
	virtual const char*		getvalue(const char* id, stringbuilder& sb) const { return 0; }
	virtual bool			isallow(const char* id) const { return true; }
	virtual bool			isfocusable() const { return true; }
	virtual bool			ismodified() const { return false; }
	virtual void			paint(const rect& rc);
	void					post(const char* id) const;
	static sprite*			std_images;
	virtual void			setvalue(const char* id, long value) {}
	int						toolbar(int x, int y, int width, int* next_x = 0) const;
};
// Abstract scrollable element. Scroll apear automatically if needed.
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
	int						origin, current;
public:
	virtual void			cell(const rect& rc, int line, int column) const {}
	int						getorigin() const { return origin; }
	int						getoriginwidth() const { return 0; }
	int						getcurrent() const { return current; }
	virtual int				getmaximum() const { return 0; }
	virtual int				getmaximumwidth() const { return 0; }
	void					paint(const rect& rc) override {}
	virtual void			row(const rect& rc, int line) const {}
	virtual int				rowheader(const rect& rc) const { return 0; }
	virtual void			rowhilite(const rect& rc, int index) const;
	virtual void			select(int index, int column);
};
struct anyreq {
	unsigned short			offset;
	unsigned char			size;
	unsigned char			bit;
};
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
class textedit : public scrollable {
	char*					string;
	unsigned				maxlenght;
	int						cashed_width;
	int						cashed_string;
	int						cashed_origin;
	int						p1, p2;
public:
	rect					rctext, rcclient;
	list*					records;
	unsigned				align;
	bool					readonly, update_records, show_records, post_escape;
	textedit(char* string, unsigned maximum_lenght, bool select_text);
	void					clear();
	virtual void			cashing(rect rc);
	void					correct();
	bool					editing(rect rc);
	void					ensurevisible(int linenumber);
	int						getbegin() const;
	const char**			getcommands() const override;
	int						getend() const;
	point					getpos(rect rc, int index, unsigned state) const;
	int						getrecordsheight() const;
	int						hittest(rect rc, point pt, unsigned state) const;
	void					invalidate() override;
	bool					isshowrecords() const;
	int						lineb(int index) const;
	int						linee(int index) const;
	int						linen(int index) const;
	bool					left(bool shift, bool ctrl);
	void					paste(const char* string);
	void					redraw(const rect& rc) override;
	bool					right(bool shift, bool ctrl);
	bool					select(int index, bool shift);
	unsigned				select_all(bool run);
	void					setcount(int value) {}
	void					setrecordlist(const char* string);
	void					setvalue(const char* id, long v) override;
	void					updaterecords(bool setfilter);
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
void						application();
bool						addbutton(rect& rc, bool focused, const char* t1, unsigned k1, const char* tt1);
int							addbutton(rect& rc, bool focused, const char* t1, unsigned k1, const char* tt1, const char* t2, unsigned k2, const char* tt2);
void						breakmodal(int result);
void						breakparam();
void						buttoncancel();
void						buttonok();
bool						button(const rect& rc, const char* title, const char* tips, unsigned key, color value, bool focused, bool checked, bool press, bool border);
void						buttonl(int& x, int y, const char* title, fnevent proc, unsigned key = 0, void* focus_value = 0);
void						buttonr(int& x, int y, const char* title, fnevent proc, unsigned key = 0);
void						cbsetint();
void						cbsetptr();
void						cbsetsource();
void						checkbox(int x, int& y, int width, void* source, int size, unsigned bits, const char* label, const char* tips = 0);
void						fieldi(int x, int& y, int width, const char* label, void* source, int size, int label_width, int digits);
void						field(int x, int& y, int width, const char* label, char* source, unsigned size, int label_width, fnchoose choosep);
void						field(int x, int& y, int width, const char* label, const char*& source, int label_width, fnchoose choosep);
int							getimage(const char* id);
unsigned					getkey(const char* id);
int							getresult();
long						getsource(void* source, int size);
void						initialize(const char* label, int timer = 0);
bool						ismodal();
void						radio(int x, int& y, int width, void* source, int size, unsigned bits, const char* label, const char* tips = 0);
void						setnext(fnevent v);
void						setposition(int& x, int& y, int& width, int padding = -1);
void						setsource(void* source, int size, long value);
void						statusbar(const char* format, ...);
void						titletext(int& x, int y, int& width, const char* label, int title = 128, const char* separator = 0);
void						tooltips(const char* format, ...);
}