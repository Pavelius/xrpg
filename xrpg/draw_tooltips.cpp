#include "draw.h"
#include "draw_button.h"
#include "draw_input.h"
#include "handler.h"
#include "stringbuilder.h"

static char				tooltips_text[4096];
static int				px, py, pw;
bool					tooltips_use_idle = true;
fnevent					tooltips_custom;
static stringbuilder	sb(tooltips_text);

HANDLER(before_modal) {
	sb.clear();
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
	rect rc = {x, y, x + w, y};
	draw::textf(rc, tooltips_text);
	rc = rc - metrics::padding;
	// Correct border
	auto height = draw::getheight();
	auto width = draw::getwidth();
	if(rc.y2 >= height)
		rc.move(0, height - 2 - rc.y2);
	if(rc.x2 >= width)
		rc.move(width - 2 - rc.x2, 0);
	// Show background
	if(tooltips_custom) {
		draw::hot.hilite = rc;
		tooltips_custom();
	} else {
		draw::rectf(rc, colors::tips::back);
		draw::rectb(rc, colors::border);
	}
	rc = rc + metrics::padding;
	// Show text
	auto push_fore = draw::fore;
	draw::fore = colors::tips::text;
	draw::textf(rc.x1, rc.y1, rc.width(), tooltips_text);
	draw::fore = push_fore;
}

void draw::tooltips(const char* format, ...) {
	sb.addv(format, xva_start(format));
}

void draw::tooltips(int x, int y, int width) {
	px = x;
	py = y;
	pw = width;
}
