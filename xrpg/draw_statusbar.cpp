#include "draw_button.h"
#include "handler.h"
#include "stringbuilder.h"

static char				sb_text[4096];
static stringbuilder	sb(sb_text);

void draw::statusbar(const char* format, ...) {
	sb.addv(format, xva_start(format));
}