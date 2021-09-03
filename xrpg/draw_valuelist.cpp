#include "draw.h"
#include "draw_focus.h"
#include "draw_valuelist.h"

using namespace draw::controls;

void valuelist::add(const char* name, size_t value, int image) {
	auto p = source.add();
	p->name = name;
	p->value = value;
	p->image = image;
}

const char*	valuelist::getcurrentname() const {
	if(!getmaximum())
		return "";
	char temp[260]; stringbuilder sb(temp);
	return getname(current, 0, sb);
}

size_t valuelist::getcurrentvalue() const {
	if(!getmaximum())
		return 0;
	return source[current].value;
}

bool valuelist::isfocused() const {
	return always_focus || draw::isfocused(this);
}