#include "draw.h"
#include "draw_focus.h"
#include "draw_valuelist.h"

using namespace draw::controls;

void valuelist::add(const char* name) {
	auto p = source.add();
	p->name = name;
}

void valuelist::appear(int x, int y) {
	pixels_per_line = getrowheight();
	auto n = getmaximum();
	if(n > 10)
		n = 10;
	auto h = n * pixels_per_line;
	client.set(x, y, x + 300, y + h);
}

const char* valuelist::getname(int line, int column, stringbuilder& sb) const {
	return source[line].name;
}

bool valuelist::isfocused() const {
	return always_focus || draw::isfocused(this);
}

void valuelist::paint(const rect& rc) {
	if(ishilite(rc))
		hot.cursor = cursor::Arrow;
	list::paint(rc);
}

//void valuelist::mouseclick() const {
//	if(hot.key == MouseLeft && hot.pressed)
//		draw::execute(post_select, current_hilite_row, getcolumn(), this);
//}