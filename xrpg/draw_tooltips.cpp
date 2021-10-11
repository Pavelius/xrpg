#include "draw.h"
#include "draw_button.h"
#include "draw_input.h"
#include "handler.h"
#include "stringbuilder.h"

using namespace draw;

static char		tooltips_text[4096];
static int		px, py, pw;
stringbuilder	tooltips_sb(tooltips_text);

void tooltips_getrect(rect& rc, int border) {
	// Calculate rect
	auto x = px, y = py;
	if(x == -1000 && y == -1000) {
		if(hot.hilite) {
			x = hot.hilite.x1 + border;
			y = hot.hilite.y2 + border + 2;
		} else {
			x = hot.mouse.x + 32;
			y = hot.mouse.y + 32;
		}
	}
	auto w = pw;
	if(!w)
		w = 300;
	rc = {x, y, x + w, y};
	draw::textf(rc, tooltips_text);
	rc = rc - border;
	// Correct border
	auto height = draw::getheight();
	auto width = draw::getwidth();
	if(rc.y2 >= height)
		rc.move(0, height - 2 - rc.y2);
	if(rc.x2 >= width)
		rc.move(width - 2 - rc.x2, 0);
}

static void tooltips_render() {
	// Show background
	rect rc; tooltips_getrect(rc, metrics::padding);
	auto push_fore = draw::fore;
	auto push_width = width;
	width = rc.width();
	fore = colors::tips::back;
	rectf(rc);
	fore = colors::border;
	rectb(rc);
	rc = rc + metrics::padding;
	// Show text
	fore = colors::tips::text;
	auto push_caret = caret;
	caret.x = rc.x1;
	caret.y = rc.y1;
	textf(tooltips_sb.begin());
	caret = push_caret;
	fore = push_fore;
	width = push_width;
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