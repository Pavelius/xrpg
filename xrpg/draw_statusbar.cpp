#include "draw.h"
#include "draw_button.h"
#include "handler.h"
#include "stringbuilder.h"

namespace {
struct sbcolumn {
	int					width;
	const char*			name;
	explicit operator bool() const { return width != 0; }
};
}

static rect				sb_rect;
static char				sb_text[4096];
static char				sb_columns[1024];
static sbcolumn			sb_coldata[10];
static stringbuilder	sb(sb_text);
static stringbuilder	sc(sb_columns);

stringbuilder& getstatustext() {
	return sb;
}

void draw::statusbar(const char* format, ...) {
	sb.addv(format, xva_start(format));
}

void draw::statusbar(rect& rb) {
	if(!font)
		return;
	sb_rect = rb;
	sb_rect.y1 = sb_rect.y2 - texth() - 3 * 2;
	rb.y2 -= sb_rect.height();
	//auto dx = font->height + 6;
	gradv(sb_rect, colors::button.lighten(), colors::button.darken());
	line(sb_rect.x1, sb_rect.y1, sb_rect.x2, sb_rect.y1, colors::border);
	sb_rect.offset(4, 3);
}

void draw::statuscolumn(int index, int width, const char* format, ...) {
	if(index >= sizeof(sb_coldata) / sizeof(sb_coldata[0]))
		return;
	sb_coldata[index].width = width;
	sb_coldata[index].name = sc.get();
	sc.addv(format, xva_start(format));
	sc.addsz();
}

HANDLER(before_modal) {
	memset(sb_coldata, 0, sizeof(sb_coldata));
	sb_rect.clear();
	sb.clear();
	sc.clear();
}

HANDLER(before_input) {
	if(!sb_rect)
		return;
	const int padding = 4;
	auto x = sb_rect.x2;
	for(auto& e : sb_coldata) {
		if(!e)
			break;
		x = x - e.width - padding;
		draw::line(x, sb_rect.y1, x, sb_rect.y2, colors::border);
		if(e.name)
			draw::text(x + padding, sb_rect.y1, e.name, -1, 0, e.width);
	}
	if(sb_text[0])
		draw::textf(sb_rect.x1, sb_rect.y1, x - sb_rect.x1, sb_text);
}