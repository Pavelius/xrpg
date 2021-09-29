#include "draw.h"
#include "draw_button.h"
#include "draw_input.h"
#include "handler.h"
#include "stringbuilder.h"

static char				tooltips_text[4096];
static int				px, py, pw;
bool					tooltips_use_idle = true;
fnevent					tooltips_custom;
stringbuilder			tooltips_sb(tooltips_text);

void tooltips_getrect(rect& rc) {
	// Calculate rect
	auto x = px, y = py;
	if(x == -1000 && y == -1000) {
		if(draw::hot.hilite) {
			x = draw::hot.hilite.x1 + metrics::padding;
			y = draw::hot.hilite.y2 + metrics::padding + 2;
		} else {
			x = draw::hot.mouse.x + 32;
			y = draw::hot.mouse.y + 32;
		}
	}
	auto w = pw;
	if(!w)
		w = 300;
	rc = {x, y, x + w, y};
	draw::textf(rc, tooltips_text);
	rc = rc - metrics::padding;
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
	if(tooltips_custom)
		tooltips_custom();
	else {
		draw::rectf(draw::hot.hilite, colors::tips::back);
		draw::rectb(draw::hot.hilite, colors::border);
	}
	draw::hot.hilite = draw::hot.hilite + metrics::padding;
	// Show text
	auto push_fore = draw::fore;
	draw::fore = colors::tips::text;
	draw::textf(draw::hot.hilite.x1, draw::hot.hilite.y1, draw::hot.hilite.width(),
        tooltips_text);
	draw::fore = push_fore;
}

HANDLER(before_modal) {
	tooltips_sb.clear();
	px = py = -1000;
	pw = 0;
}

HANDLER(before_input) {
	if(!tooltips_text[0])
		return;
	if(!draw::font)
		return;
	if(tooltips_use_idle && draw::hot.key != InputIdle)
		return;
    tooltips_getrect(draw::hot.hilite);
    tooltips_render();
}

void draw::tooltips(const char* format, ...) {
	tooltips_sb.addv(format, xva_start(format));
}

void draw::tooltips(int x, int y, int width) {
	px = x;
	py = y;
	pw = width;
}