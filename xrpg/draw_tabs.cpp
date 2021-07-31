#include "crt.h"
#include "view.h"

using namespace draw;

int draw::sheetline(rect rc, bool background) {
	if(background)
		gradv(rc, colors::border.lighten(), colors::border.darken());
	rectf({rc.x1, rc.y2 - 2, rc.x2, rc.y2}, colors::active);
	return rc.height();
}

static bool sheet(rect& rc, rect& rct, const char* string, bool* area_result, bool checked, bool right_side, int w2) {
	bool result = false;
	auto tw = textw(string);
	auto ts = textw('1');
	auto width = tw + w2;
	if(right_side)
		rct.set(rc.x2 - width - ts * 2, rc.y1, rc.x2, rc.y2);
	else
		rct.set(rc.x1, rc.y1, rc.x1 + width + ts * 2, rc.y2);
	auto a = ishilite(rct);
	if(area_result)
		*area_result = a;
	auto push_fore = fore;
	auto ty = rct.y1 + 2;
	if(checked) {
		rectf({rct.x1 + 1, rct.y1 + 1, rct.x2, rct.y2 + 1}, colors::form);
		fore = colors::border;
		line(rct.x1, rct.y1, rct.x2, rct.y1);
		line(rct.x1, rct.y1, rct.x1, rct.y2);
		line(rct.x2, rct.y1, rct.x2, rct.y2);
		fore = colors::active;
		ty += 1;
	}
	text(rct.x1 + ts, ty, string, -1, 0);
	fore = push_fore;
	if(a) {
		if(hot.key == MouseLeft && hot.pressed)
			result = true;
		else
			draw::rectf({rct.x1 + 1, rct.y1 + 1, rct.x2, rct.y2 - 2}, colors::form, 64);
	}
	if(right_side)
		rc.x2 -= rct.width();
	else
		rc.x1 += rct.width();
	return result;
}

int draw::tabs(rect rc, bool show_close, bool right_side, void** data, int start, int count, int current, int* hilite,
	fntext gtext, int* result_x1) {
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
		if(sheet(rc, element, s, &a, (i == current), right_side, (show_close ? 16 : 0)))
			result = 1;
		if(a) {
			if(hilite)
				*hilite = i;
		}
		if((a || (i == current)) && show_close) {
			const int dy = 12;
			rect rcx;
			rcx.y1 = element.y1 + (element.height() - dy) / 2;
			rcx.y2 = rcx.y1 + dy;
			rcx.x1 = element.x2 - dy - 4;
			rcx.x2 = rcx.x1 + rcx.height();
			//if(ishilite(rcx)) {
			//	if(buttonh(rcx, false, false, false, true, 0))
			//		result = 2;
			//	tooltips("�������");
			//}
			line(rcx.x1 + 4, rcx.y1 + 4, rcx.x2 - 4, rcx.y2 - 4, fore);
			line(rcx.x2 - 4, rcx.y1 + 4, rcx.x1 + 4, rcx.y2 - 4, fore);
		}
	}
	if(result_x1)
		*result_x1 = rc.x1;
	return result;
}

int draw::tabv(rect ro, bool show_close, bool right_side, void** data, int start, int count, int current, int* hilite, fntext gtext) {
	auto dy = draw::texth() + gui.padding;
	rect rc = {ro.x1, ro.y1 + dy, ro.x2, ro.y2};
	rectf(rc, colors::form);
	rectb(rc, colors::border);
	return tabs({ro.x1, ro.y1, ro.x2, ro.y1 + dy}, show_close, right_side, data, start, count, current, hilite, gtext);
}