#include "crt.h"
#include "draw.h"

#pragma once

typedef bool(*fnvisible)(const void* object);

namespace draw {
enum class visibility : unsigned char { Icon, Text, IconAndText };
enum class widthtype : unsigned char { Default, Resized, Fixed, Inner, Auto, };
enum class selection : unsigned char { Cell, Text, Row, };
enum class dock : unsigned char { Left, LeftBottom, Right, RightBottom, Bottom, Workspace };
enum class totaltype : unsigned char { None, Summarize, Maximum, Minimum, Average };
struct docki {
	const char*				id;
};
namespace controls {
struct control;
struct visual;
struct anyreq {
	unsigned short			offset;
	unsigned char			size;
	unsigned char			bit;
	constexpr bool operator==(const anyreq& e) { return e.offset == offset; }
	int						get(const void* object) const;
	const char*				gets(const void* object) const;
	constexpr char*			ptr(const void* object) const { return (char*)object + offset; }
	void					set(void* object, int value) const;
};
struct command {
	class builder {
		void				render(const control* parent, const command* commands, bool& separator, int& count);
	public:
		builder() = default;
		virtual ~builder() {}
		virtual void		add(const control* parent, const command& cmd) = 0;
		virtual void		addseparator() = 0;
		virtual const command* finish() const { return 0; }
		void				render(const control* parent, const command* commands);
		virtual void		start() const {}
	};
	const char*				panel;
	const char**			commands;
};
struct control {
	int						splitter;
	constexpr control() : splitter(0) {}
	virtual ~control() {}
	void					contextmenu(const command* source);
	void					contextmenu(const command* source, command::builder& builder);
	static command			commands_edit[];
	virtual void			execute(const char* id) {}
	virtual command*		getcommands() const { return 0; }
	virtual const char*		getvalue(const char* id, stringbuilder& sb) const {}
	virtual bool			isallow(const char* id) const {}
	virtual bool			ismodified() const { return false; }
	virtual void			paint(const rect& rc) const {}
	int						toolbar(int x, int y, int width, int* next_x = 0) const;
};
struct list : public control {
	int						origin, current;
	virtual void			cell(const rect& rc, int line, int column) const {}
	int						getorigin() const { return origin; }
	int						getoriginwidth() const { return 0; }
	int						getcurrent() const { return current; }
	virtual int				getmaximum() const { return 0; }
	virtual int				getmaximumwidth() const { return 0; }
	void					paint(const rect& rc) const override {}
	virtual void			row(const rect& rc, int line) const {}
	virtual int				rowheader(const rect& rc) const { return 0; }
	virtual void			rowhilite(const rect& rc, int index) const;
	virtual void			select(int index, int column);
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
void						setposition(int& x, int& y, int& width, int padding = -1);
}