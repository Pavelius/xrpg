#include "color.h"
#include "point.h"

#pragma once

namespace draw {
struct uieffect {
	point			position, start, finish;
	point*			save_position;
	const char*		format;
	color			fore;
	unsigned char	alpha;
	int				alpha_start, alpha_finish;
	int				value; // Use with format string
	unsigned		time_start, time_finish;
	explicit operator bool() const { return time_start < time_finish; }
	static uieffect* add();
	static uieffect* add(point postition, const char* format, color fore, int duration);
	void			clear();
	static void		clearall();
	static void		fixwait(const uieffect* p);
	bool			isactive() const;
	void			paint() const;
	void			setduration(int duration);
	void			set(point v) { position = v; }
	void			set(color v) { fore = v; }
	void			set(const char* v) { format = v; }
	void			setgoal(point v);
	void			setgoalalpha(int v);
	void			setlinked(point& v);
	void			setvalue(int v) { value = v; }
	void			stop();
	void			update();
	void			wait() { fixwait(this); }
	static void		waitall() { fixwait(0); }
};
}
