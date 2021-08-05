#include "draw.h"
#include "draw_button.h"
#include "handler.h"
#include "stringbuilder.h"

static rect				sb_rect;
static char				sb_text[4096];
static stringbuilder	sb(sb_text);

void draw::statusbar(const char* format, ...) {
	sb.addv(format, xva_start(format));
}

void draw::statusbar(rect& rb) {
	if(!font)
		return;
	sb_rect = rb;
	sb_rect.y1 = sb_rect.y2 - texth() - 3 * 2;
	rb.y2 -= sb_rect.height();
	auto dx = font->height + 6;
	gradv(sb_rect, colors::button.lighten(), colors::button.darken());
	line(sb_rect.x1, sb_rect.y1, sb_rect.x2, sb_rect.y1, colors::border);
	sb_rect.offset(4, 3);
}

HANDLER(before_modal) {
	sb_rect.clear();
	sb.clear();
}

HANDLER(before_input) {
	if(!sb_rect)
		return;
	if(sb_text[0])
		draw::textf(sb_rect.x1, sb_rect.y1, sb_rect.width(), sb_text);
}