#include "crt.h"
#include "draw.h"

using namespace draw;

int draw::tab_pixels = 0;
static int original_x1;

static void(*draw_icon)(int& x, int& y, int x0, int x2, int* max_width, int& w, const char* name);

static bool match(const char** string, const char* name) {
	int n = zlen(name);
	if(memcmp(*string, name, n) != 0)
		return false;
	(*string) += n;
	return true;
}

static const char* glink(const char* p, char* result, unsigned result_maximum) {
	result[0] = 0;
	if(*p == '\"') {
		auto sym = *p++;
		stringbuilder sb(result, result + result_maximum);
		p = sb.psstr(p, sym);
	} else if(*p == '(') {
		auto ps = result;
		auto pe = ps + result_maximum;
		while(*p && *p != ')') {
			if(ps < pe)
				*ps++ = *p;
			p++;
		}
		*ps++ = 0;
		if(*p == ')')
			p++;
	}
	return p;
}

static int gettabwidth() {
	return tab_pixels ? tab_pixels : textw(' ') * 4;
}

static const char* textspc(const char* p) {
	int tb;
	while(true) {
		switch(p[0]) {
		case ' ':
			p++;
			caret.x += draw::textw(' ');
			continue;
		case '\t':
			p++;
			tb = gettabwidth();
			caret.x = original_x1 + ((caret.x - original_x1 + tb) / tb) * tb;
			continue;
		}
		break;
	}
	return p;
}

static const char* word(const char* text) {
	while(((unsigned char)*text) > 0x20 && *text != '*' && *text != '[' && *text != ']')
		text++;
	return text;
}

static void apply_line_feed(int x1) {
	width_maximum = imax(width_maximum, caret.x - original_x1);
	caret.x = x1;
	caret.y += texth();
}

static const char* textfln(const char* p, int x1, color c1) {
	char temp[4096];
	int x2 = original_x1 + width;
	unsigned flags = 0;
	draw::fore = c1;
	temp[0] = 0;
	while(true) {
		if(p[0] == '*' && p[1] == '*') {
			p += 2;
			if(flags & TextBold)
				flags &= ~TextBold;
			else
				flags |= TextBold;
			continue;
		} else if(p[0] == '*') {
			p++;
			if(flags & TextItalic)
				flags &= ~TextItalic;
			else {
				if((flags & TextItalic) == 0)
					caret.x += texth() / 3;
				flags |= TextItalic;
			}
			continue;
		} else if(p[0] == '[' && p[1] == '[')
			p++;
		else if(p[0] == ']' && p[1] == ']')
			p++;
		else if(p[0] == '[') {
			p++;
			switch(*p) {
			case '~':
				p++;
				fore = colors::text.mix(colors::window, 64);
				break;
			case '+':
				p++;
				fore = colors::green;
				break;
			case '-':
				p++;
				fore = colors::red;
				break;
			case '!':
				p++;
				fore = colors::yellow;
				break;
			case '#':
				p++;
				flags |= TextUscope;
				fore = colors::special;
				break;
			default:
				fore = colors::special;
				break;
			}
			p = glink(p, temp, sizeof(temp) / sizeof(temp[0]) - 1);
		} else if(p[0] == ']') {
			p++;
			fore = c1;
			temp[0] = 0;
			flags &= ~TextUscope;
		}
		// Обработаем пробелы и табуляцию
		p = textspc(p);
		int w;
		if(p[0] == ':' && p[1] >= 'a' && p[1] <= 'z') {
			p++;
			char temp[128]; stringbuilder sb(temp);
			p = sb.psidf(p);
			if(*p == ':')
				p++;
			w = 0;
		} else {
			const char* p2 = word(p);
			w = textw(p, p2 - p);
			if(caret.x + w > x2)
				apply_line_feed(x1);
			text(caret.x, caret.y, p, p2 - p, flags);
			p = p2;
		}
		int x4 = caret.x;
		caret.x += w;
		p = textspc(p);
		if(temp[0] || (flags & TextUscope) != 0) {
			int x3 = imin(x2, original_x1);
			int y2 = caret.y + texth();
			if(flags & TextUscope)
				line(x4, y2, x3, y2);
			rect rc = {x4, caret.y, x3, y2};
			if(ishilite(rc)) {
				if(flags & TextUscope) {
					hot.cursor = cursor::Hand;
					if(temp[0] && hot.key == MouseLeft && !hot.pressed)
						zcpy(link, temp, sizeof(link) - 1);
				} else
					zcpy(link, temp, sizeof(link) - 1);
			}
		}
		// Line feed
		if(p[0] == 0 || p[0] == 10 || p[0] == 13) {
			p = skipcr(p);
			apply_line_feed(x1);
			break;
		}
	}
	width_maximum = imax(width_maximum, caret.x - original_x1);
	return p;
}

void draw::textf(const char* string, int min_height, int* cashe_height, const char** cashe_string) {
	width_maximum = 0;
	auto push_fore = fore;
	auto push_font = font;
	color color_text = fore;
	const char* p = string;
	original_x1 = caret.x;
	int y0 = caret.y;
	if(cashe_height) {
		*cashe_string = p;
		*cashe_height = 0;
	}
	while(p[0]) {
		int mw2 = 0;
		if(cashe_height && (caret.y - y0) <= min_height) {
			*cashe_string = p;
			*cashe_height = caret.y - y0;
		}
		if(match(&p, "#--")) { // Header 3
			font = metrics::small;
			p = textfln(skipsp(p), original_x1, colors::h3);
		} else if(match(&p, "###")) { // Header 3
			font = metrics::h3;
			p = textfln(skipsp(p), original_x1, colors::h3);
		} else if(match(&p, "##")) {// Header 2
			font = metrics::h2;
			p = textfln(skipsp(p), original_x1, colors::h2);
		} else if(match(&p, "#")) { // Header 1
			font = metrics::h1;
			p = textfln(skipsp(p), original_x1, colors::h1);
		} else if(match(&p, "---")) { // Line
			p = skipspcr(p);
			caret.y += 2;
			line(caret.x, caret.y, caret.x + width, caret.y);
			caret.y += 2;
		} else if(match(&p, "...")) { // Без форматирования
			p = skipcr(p);
			font = metrics::font;
			color c1 = colors::window.mix(colors::border, 256 - 32);
			caret.y += texth() / 2;
			while(p[0]) {
				int c = textbc(p, width);
				if(!c)
					break;
				auto push_fore = fore;
				fore = c1;
				rectf({caret.x, caret.y, caret.x + width, caret.y + texth()});
				fore = push_fore;
				text(caret.x, caret.y, p, c);
				caret.y += texth();
				p += c;
				if(match(&p, "...")) {
					p = skipcr(p);
					caret.y += texth() / 2;
					break;
				}
			}
		} else if(match(&p, "* ")) {
			// Список
			int dx = texth() / 2;
			int rd = texth() / 6;
			circlef(caret.x + dx + 2, caret.y + dx, rd);
			circle(caret.x + dx + 2, caret.y + dx, rd);
			auto push_width = width;
			auto x1 = caret.x;
			caret.x += texth(); width -= texth();
			p = textfln(p, caret.x, color_text);
			caret.x = x1;
			width = push_width;
		} else
			p = textfln(p, original_x1, color_text);
		// Возвратим стандартные настройки блока
		font = metrics::font;
		fore = color_text;
	}
	fore = push_fore;
	font = push_font;
}

int draw::textf(rect& rc, const char* string) {
	auto push_clipping = clipping;
	clipping.clear();
	auto push_width = width;
	auto push_caret = caret; caret = {};
	width = rc.width();
	draw::textf(string);
	rc.x2 = rc.x1 + width_maximum;
	rc.y2 = rc.y2 + caret.y;
	width = push_width;
	caret = push_caret;
	clipping = push_clipping;
	return rc.height();
}