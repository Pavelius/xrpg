#include "draw_builder.h"

using namespace draw;

void builder::render(const char** commands) {
	if(!commands)
		return;
	for(auto p = commands; *p; p++) {
		auto id = *p;
		if(getcommands && id[0] == '#') {
			if(allowid && !allowid(object, id+1))
				continue;
			separator = true;
			render(getcommands(object, id + 1));
			separator = true;
		} else {
			if(separator) {
				if(count > 0)
					addseparator();
				separator = false;
			}
			if(add(*p))
				count++;
		}
	}
}

void builder::render(const char** commands, const void* object, fnallowid allowid, fngetcommands getcommands, bool separator) {
	this->separator = separator;
	this->count = separator ? 1 : 0;
	this->object = object;
	this->allowid = allowid;
	this->getcommands = getcommands;
	render(commands);
}

const char* builder::choose(const char** commands, const void* object, fnallowid allowid, fngetcommands getcommands) {
	if(!commands)
		return 0;
	start();
	render(commands, object, allowid, getcommands);
	return finish();
}