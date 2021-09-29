#include "draw.h"
#include "draw_button.h"
#include "draw_input.h"
#include "handler.h"
#include "stringbuilder.h"

static char				tooltips_text[4096];
static int				px, py, pw;
bool					tooltips_use_idle = true;
stringbuilder			tooltips_sb(tooltips_text);

void tooltips_getrect(rect& rc, int border) {
	// Calculate rect
	auto x = px, y = py;
	if(x == -1000 && y == -1000) {
		if(draw::hot.hilite) {
			x = draw::hot.hilite.x1 + border;
			y = draw::hot.hilite.y2 + border + 2;
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
	draw::rectf(rc, colors::tips::back);
	draw::rectb(rc, colors::border);
	rc = rc + metrics::padding;
	// Show text
	auto push_fore = draw::fore;
	draw::fore = colors::tips::text;
	draw::textf(rc.x1, rc.y1, rc.width(), tooltips_sb.begin());
	draw::fore = push_fore;
}

fnevent	tooltips_custom = tooltips_render;

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
	if(tooltips_custom)
		tooltips_custom();
}

void draw::tooltips(const char* format, ...) {
	tooltips_sb.addv(format, xva_start(format));
}

void draw::tooltips(int x, int y, int width) {
	px = x;
	py = y;
	pw = width;
}