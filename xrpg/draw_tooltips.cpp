#include "draw.h"
#include "draw_button.h"
#include "handler.h"
#include "stringbuilder.h"

static char				sb_text[4096];
static rect				sb_rect;
static stringbuilder	sb(sb_text);

HANDLER(before_modal) {
	sb.clear();
	sb_rect.clear();
}

HANDLER(before_input) {
	if(!sb_text[0])
		return;
	if(draw::hot.key != InputIdle)
		return;
	auto push_font = draw::font;
	auto push_fore = draw::fore;
	draw::font = metrics::font;
	if(draw::font) {
		// Calculate rect
		rect rc = sb_rect;
		if(!rc) {
			if(draw::hot.hilite) {
				rc.y1 = draw::hot.hilite.y2 + metrics::padding + 2;
				rc.y2 = draw::hot.hilite.y2 + metrics::padding + 2;
				rc.x1 = draw::hot.hilite.x1 + metrics::padding;
				rc.x2 = rc.x1 + 300;
			} else {
				rc.x1 = draw::hot.mouse.x + 32;
				rc.y1 = draw::hot.mouse.y + 32;
				rc.x2 = rc.x1 + 300;
				rc.y2 = rc.y1;
			}
		}
		if(rc.y1 == rc.y2)
			draw::textf(rc, sb_text);
		rc = rc - metrics::padding;
		// Correct border
		int height = draw::getheight();
		int width = draw::getwidth();
		if(rc.y2 >= height)
			rc.move(0, height - 2 - rc.y2);
		if(rc.x2 >= width)
			rc.move(width - 2 - rc.x2, 0);
		// Show background
		draw::rectf(rc, colors::tips::back);
		draw::rectb(rc, colors::border);
		rc = rc + metrics::padding;
		// Show text
		draw::fore = colors::tips::text;
		draw::textf(rc.x1, rc.y1, rc.width(), sb_text);
	}
	draw::fore = push_fore;
	draw::font = push_font;
}

void draw::tooltips(const char* format, ...) {
	sb.addv(format, xva_start(format));
}