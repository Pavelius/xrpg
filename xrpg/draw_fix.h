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
	unsigned long	time_start, time_finish;
	int				alpha_start, alpha_finish;
	int				value;
	explicit operator bool() const { return time_start < time_finish; }
	static uieffect* add();
	static uieffect* add(point postition, const char* format, color fore, int duration);
	void			clear();
	static void		clearall();
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
	void			wait();
	static void		waitall();
};
}
