#include "draw.h"
#include "draw_focus.h"
#include "draw_valuelist.h"

using namespace draw::controls;

void valuelist::add(const char* name) {
	auto p = source.add();
	p->name = name;
}

const char* valuelist::getname(int line, int column, stringbuilder& sb) const {
	return source[line].name;
}

const char*	valuelist::getcurrentname() const {
	if(!getmaximum())
		return "";
	char temp[260]; stringbuilder sb(temp);
	return getname(current, 0, sb);
}

bool valuelist::isfocused() const {
	return always_focus || draw::isfocused(this);
}