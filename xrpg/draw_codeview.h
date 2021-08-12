#include "draw_control.h"

#pragma once

namespace draw {
namespace controls {
class codeview : public control {
	int						cash_origin;
	int						lines_per_page;
	int						p1, p2;
	point					pos1, pos2, size;
	int						origin_x, origin_y;
	int						maximum_x, maximum_y;
	rect					rctext;
	bool					readonly;
	static const sprite*	font;
	static point			fontsize;
public:
	codeview();
	void					clear();
	void					correction();
	void					ensurevisible(int linenumber);
	int						getbegin() const;
	point					getbeginpos() const;
	const char**			getcommands() const override;
	int						getcurrent() const { return p1; }
	point					getcurrentpos() const { return pos1; }
	int						getend() const;
	point					getendpos() const;
	int						getpixelperline() const { return fontsize.y; }
	int						hittest(rect rc, point pt, unsigned state) const;
	void					invalidate();
	bool					isselected() const { return p2 != -1 && p1 != -1; }
	void					paint(const rect& rc) override;
	void					paste(const char* input);
	void					pastecr();
	void					pastetab();
	void					redraw(const rect& rc);
	void					right(bool shift, bool ctrl);
	void					set(int index, bool shift);
	bool					wordselect(bool run);
};
}
}
