#include "draw.h"
#include "draw_button.h"
#include "handler.h"
#include "stringbuilder.h"

static char				sb_text[4096];
static stringbuilder	sb(sb_text);

HANDLER(before_input) {
	if(sb_text[0]) {
		sb_text[0] = 0;
	}
}

void draw::tooltips(const char* format, ...) {
	sb.addv(format, xva_start(format));
}