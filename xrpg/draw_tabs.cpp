#include "crt.h"
#include "draw.h"
#include "draw_button.h"

using namespace draw;

int draw::sheetline(rect rc, bool background) {
	if(background)
		gradv(colors::border.lighten(), colors::border.darken());
	auto push_fore = fore;
	fore = colors::active;
	rectf();
	fore = push_fore;
	return rc.height();
}

static bool sheet(rect& rc, rect& rct, const char* string, bool* area_result, bool checked, bool right_side, int w2, color back) {
	bool result = false;
	auto tw = textw(string);
	auto ts = textw('1');
	auto width = tw + w2;
	if(right_side)
		rct.set(rc.x2 - width - ts * 2, rc.y2 - texth() - 7, rc.x2, rc.y2);
	else
		rct.set(rc.x1, rc.y2 - texth() - 7, rc.x1 + width + ts * 2, rc.y2);
	auto a = ishilite(rct);
	if(area_result)
		*area_result = a;
	auto push_fore = fore;
	auto ty = rct.y1 + 4;
	if(checked) {
		fore = back;
		rectf();
		fore = colors::border;
		//line(rct.x1, rct.y1, rct.x2, rct.y1);
		//line(rct.x1, rct.y1, rct.x1, rct.y2);
		//line(rct.x2, rct.y1, rct.x2, rct.y2);
		fore = colors::active;
		ty += 1;
	}
	auto push_caret = caret;
	caret.x = rct.x1 + ts;
	caret.y = ty;
	text(string, -1, 0);
	caret = push_caret;
	if(a) {
		if(hot.key == MouseLeft && hot.pressed)
			result = true;
		else {
			fore = back;
			auto push_alpha = alpha;
			alpha = 64;
			draw::rectf();
			alpha = push_alpha;
		}
	}
	fore = push_fore;
	if(right_side)
		rc.x2 -= rct.width();
	else
		rc.x1 += rct.width();
	return result;
}

int draw::tabs(rect rc, bool show_close, bool right_side, void** data, int start, int count, int current, int* hilite,
	fntext gtext, int* result_x1, color back) {
	char temp[260]; stringbuilder sb(temp);
	auto result = 0;
	if(hilite)
		*hilite = -1;
	for(int i = start; i < count; i++) {
		if(rc.x1 >= rc.x2)
			break;
		auto object = data[i]; sb.clear();
		auto s = gtext(object, sb);
		auto a = false; rect element;
		if(sheet(rc, element, s, &a, (i == current), right_side, (show_close ? 16 : 0), back))
			result = 1;
		if(a) {
			if(hilite)
				*hilite = i;
		}
		if((a || (i == current)) && show_close) {
			const int dy = 12;
			rect rcx;
			rcx.y1 = element.y1 + (element.height() - dy) / 2;
			if(i == current)
				rcx.y1++;
			rcx.y2 = rcx.y1 + dy;
			rcx.x1 = element.x2 - dy - 4;
			rcx.x2 = rcx.x1 + rcx.height();
			if(ishilite(rcx)) {
				if(button(rcx, 0, 0, 0, colors::button, false, false, false, true))
					result = 2;
				tooltips(getnm("Close"));
			}
			//line(rcx.x1 + 4, rcx.y1 + 4, rcx.x2 - 4, rcx.y2 - 4);
			//line(rcx.x2 - 4, rcx.y1 + 4, rcx.x1 + 4, rcx.y2 - 4);
		}
	}
	if(result_x1)
		*result_x1 = rc.x1;
	return result;
}

int draw::tabv(rect ro, bool show_close, bool right_side, void** data, int start, int count, int current, int* hilite, fntext gtext) {
	auto dy = draw::texth() + metrics::padding;
	rect rc = {ro.x1, ro.y1 + dy, ro.x2, ro.y2};
	auto push_fore = fore;
	fore = colors::form;
	rectf();
	fore = colors::border;
	rectb();
	fore = push_fore;
	return tabs({ro.x1, ro.y1, ro.x2, ro.y1 + dy}, show_close, right_side, data, start, count, current, hilite, gtext, 0, colors::form);
}