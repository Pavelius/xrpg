#include "draw.h"
#include "draw_button.h"
#include "handler.h"
#include "stringbuilder.h"

using namespace draw;

static char		tooltips_text[4096];
static int		px, py, pw;
stringbuilder	tooltips_sb(tooltips_text);

void tooltips_getrect() {
	width = 320;
	textfs(tooltips_text);
	// Calculate rect
	caret.x = px, caret.y = py;
	if(caret.x == -1000 && caret.y == -1000) {
		if(hot.hilite) {
			caret.x = hot.hilite.x1;
			caret.y = hot.hilite.y2 + 2;
		} else {
			caret.x = hot.mouse.x + 32;
			caret.y = hot.mouse.y + 32;
		}
	}
}

static void tooltips_render() {
	auto push_fore = draw::fore;
	rectpush push;
	tooltips_getrect();
	fore = colors::tips::back;
	setoffset(-metrics::padding, -metrics::padding);
	rectf();
	fore = colors::border;
	rectb();
	setoffset(metrics::padding, metrics::padding);
	// Show text
	fore = colors::tips::text;
	textf(tooltips_sb.begin());
	fore = push_fore;
}

void draw::tooltips(const char* format, ...) {
	tooltips_sb.addv(format, xva_start(format));
}

void draw::tooltips(int x, int y, int width) {
	px = x;
	py = y;
	pw = width;
}

void draw::tooltipsbeforemodal() {
	tooltips_sb.clear();
	px = py = -1000;
	pw = 0;
}

void draw::tooltipspaint() {
	if(!draw::font)
		return;
	if(!tooltips_text[0])
		return;
	if(draw::hot.key != InputIdle)
		return;
	tooltips_render();
}