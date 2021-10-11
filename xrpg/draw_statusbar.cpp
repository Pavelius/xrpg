#include "draw.h"
#include "draw_button.h"
#include "draw_input.h"
#include "handler.h"
#include "stringbuilder.h"

using namespace draw;

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
	auto push_fore = fore;
	fore = colors::border;
	line(sb_rect.x1, sb_rect.y1, sb_rect.x2, sb_rect.y1);
	sb_rect.offset(4, 3);
	fore = push_fore;
}

void draw::statuscolumn(int index, int width, const char* format, ...) {
	if(index >= sizeof(sb_coldata) / sizeof(sb_coldata[0]))
		return;
	sb_coldata[index].width = width;
	sb_coldata[index].name = sc.get();
	sc.addv(format, xva_start(format));
	sc.addsz();
}

void draw::statusbeforemodal() {
	memset(sb_coldata, 0, sizeof(sb_coldata));
	sb_rect.clear();
	sb.clear();
	sc.clear();
}

void draw::statuspaint() {
	if(!sb_rect)
		return;
	const int padding = 4;
	auto push_caret = caret;
	auto push_width = width;
	caret.x = sb_rect.x2;
	for(auto& e : sb_coldata) {
		if(!e)
			break;
		width = e.width;
		caret.x = caret.x - width - padding;
		auto push_fore = fore;
		fore = colors::border;
		line(caret.x, sb_rect.y1, caret.x, sb_rect.y2);
		fore = push_fore;
		if(e.name) {
			caret.x += padding;
			caret.y = sb_rect.y1;
			textc(e.name, -1, 0);
		}
	}
	if(sb_text[0]) {
		width = caret.x - sb_rect.x1;
		caret.x = sb_rect.x1;
		caret.y = sb_rect.y1;
		textf(sb_text);
	}
	width = push_width;
	caret = push_caret;
}