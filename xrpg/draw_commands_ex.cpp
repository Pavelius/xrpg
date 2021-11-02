#include "draw.h"
#include "draw_simple.h"
#include "draw_commands.h"

using namespace draw;

command* draw::commands::source;

static void panel(const command* source) {
	for(auto p = source; *p; p++) {
		if(pwindow == p->proc)
			continue;
		if(button(getnm(p->id), p->key, buttonfd))
			execute(cbsetptr, (long)p->proc, 0, &pwindow);
	}
}

void draw::commands::paint() {
	if(!source)
		return;
	setposld();
	panel(source);
}