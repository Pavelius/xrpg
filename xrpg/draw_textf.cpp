#include "crt.h"
#include "draw.h"

using namespace draw;

int draw::tab_pixels = 0;
long draw::text_params[16];
static const char* text_start_string;
static int text_start_horiz;
static int original_x1;
static int original_x2;

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
	if(text[0] == ':')
		return text + 1;
	while(((unsigned char)*text) > 0x20 && *text != '*' && *text != '[' && *text != ']' && *text != ':')
		text++;
	return text;
}

static void apply_line_feed(int x1) {
	if(width_maximum < caret.x - original_x1)
		width_maximum = caret.x - original_x1;
	caret.x = x1;
	caret.y += texth();
}

static const char* textfln(const char* p, int x1, color new_fore, const sprite* new_font) {
	auto push_fore = fore;
	auto push_font = font;
	char temp[4096]; temp[0] = 0;
	unsigned flags = 0;
	fore = new_fore;
	font = new_font;
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
			temp[0] = 0;
			flags &= ~TextUscope;
		}
		// ќбработаем пробелы и табул€цию
		p = textspc(p);
		int w = 0;
		if(p[0] == ':' && p[1] >= '0' && p[1] <= '9') {
			auto index = 0;
			p = stringbuilder::read(p + 1, index);
			if(*p == ':')
				p++;
			if(metrics::icons) {
				auto h = metrics::icons->get(0).sy;
				image(caret.x + 1, caret.y + (font->ascend - h) / 2 + 1, metrics::icons, index, ImageNoOffset);
				w = metrics::icons->get(0).sx + 2;
			}
		} else {
			const char* p2 = word(p);
			w = textw(p, p2 - p);
			if(caret.x + w > original_x2)
				apply_line_feed(x1);
			text(p, p2 - p, flags);
			p = p2;
		}
		int x4 = caret.x;
		caret.x += w;
		p = textspc(p);
		if(temp[0] || (flags & TextUscope) != 0) {
			auto x3 = imin(original_x2, original_x1);
			auto y2 = caret.y + texth();
			//if(flags & TextUscope)
			//	line(x4, y2, x3, y2);
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
		if(p[0] == 0 || p[0] == 10 || p[0] == 13) {
			p = skipcr(p);
			apply_line_feed(x1);
			break;
		}
	}
	if(width_maximum < caret.x - original_x1)
		width_maximum = caret.x - original_x1;
	fore = push_fore;
	font = push_font;
	return p;
}

static void imageb(int x, int y, const sprite* ps, int id, unsigned flags, const char* tips) {
	image(x, y, ps, id, flags|ImageNoOffset);
	rectpush push;
	auto push_fore = fore;
	caret.x = x; caret.y = y;
	width = ps->get(id).sx;
	height = ps->get(id).sy;
	fore = colors::border;
	rectb();
	fore = push_fore;
	if(tips && ishilite()) {
		stringbuilder sb(link);
		sb.add(tips);
	}
}

static const char* text_block(const char* p);

static const char* richimage(const char* p) {
	auto name = (const char*)text_params[0];
	auto id = text_params[1];
	auto folder = (const char*)text_params[2];
	auto tips = (const char*)text_params[3];
	auto align = (const char*)text_params[4];
	auto flags = 0;
	if(!folder)
		folder = "art/pictures";
	auto ps = gres(name, folder);
	if(!ps)
		return p;
	auto w = ps->get(id).sx;
	auto h = ps->get(id).sy;
	if(!align) {
		align = "center";
		if(w < ((original_x2 - caret.x) / 3))
			align = "left";
	}
	auto y2 = caret.y + h;
	if(equal(align, "right")) {
		imageb(original_x2 - w, caret.y, ps, id, flags, tips);
		auto push_x2 = original_x2;
		original_x2 = original_x2 - w;
		p = text_block(p);
		original_x2 = push_x2;
	} else if(equal(align, "left")) {
		imageb(caret.x, caret.y, ps, id, flags, tips);
		auto push_x1 = original_x1;
		caret.x = original_x1 + w + metrics::border;
		original_x1 = caret.x;
		p = text_block(p);
		caret.x = push_x1;
		original_x1 = push_x1;
	} else {
		imageb(caret.x, caret.y, ps, id, flags, tips);
		caret.y += h + font->descend;
	}
	if(caret.y < y2)
		caret.y = y2;
	return p;
}

static const char* parse_command(const char* p) {
	char temp[512]; stringbuilder sb(temp);
	memset(text_params, 0, sizeof(text_params));
	long count = 0;
	p = sb.psidf(p); sb.addsz();
	p = skipsp(p);
	while(*p && !(*p == 13 || *p == 10)) {
		auto p1 = p;
		p = stringbuilder::read(p, text_params[count]);
		if(p == p1) {
			text_params[count] = (long)sb.get();
			if(p[0] == '\"' || p[0] == '\'')
				p = sb.psstr(p + 1, p[0]);
			else
				p = sb.psidf(p);
			sb.addsz();
		}
		p = skipsp(p);
		if(p == p1)
			break;
		count++;
	}
	p = skipspcr(p);
	if(equal(temp, "image"))
		p = richimage(p);
	return p;
}

static const char* text_block(const char* p) {
	while(p[0]) {
		if(caret.y < clipping.y1) {
			text_start_string = p;
			text_start_horiz = caret.y - clipping.y1;
		}
		if(match(&p, "#--")) // Header small
			p = textfln(skipsp(p), original_x1, colors::h3, metrics::small);
		else if(match(&p, "###")) // Header 3
			p = textfln(skipsp(p), original_x1, colors::h3, metrics::h3);
		else if(match(&p, "##")) // Header 2
			p = textfln(skipsp(p), original_x1, colors::h2, metrics::h2);
		else if(match(&p, "#")) // Header 1
			p = textfln(skipsp(p), original_x1, colors::h1, metrics::h1);
		else if(match(&p, "---")) { // Line
			p = skipspcr(p);
			auto push_x = caret.x;
			caret.y += 2;
			caret.x = original_x1 - metrics::border;
			line(original_x2 + metrics::border, caret.y);
			caret.x = push_x;
			caret.y += 2;
		} else if(match(&p, "* ")) {
			// —писок
			auto dx = texth() / 2;
			auto rd = texth() / 6;
			auto push_caret = caret;
			caret.x += dx + 2;
			caret.y += dx;
			circlef(rd);
			circle(rd);
			caret = push_caret;
			caret.x += texth();
			p = textfln(p, caret.x, fore, font);
			caret.x = push_caret.x;
		} else if(match(&p, "$"))
			p = parse_command(p);
		else
			p = textfln(p, original_x1, fore, font);
	}
	return p;
}

void draw::textf(const char* p) {
	width_maximum = height_maximum = 0;
	text_start_string = 0;
	text_start_horiz = 0;
	original_x1 = caret.x;
	original_x2 = original_x1 + width;
	auto y0 = caret.y;
	p = text_block(p);
	if(height_maximum < caret.y - y0)
		height_maximum = caret.y - y0;
}

int draw::textfs(const char* string) {
	auto push_clipping = clipping;
	clipping.clear();
	auto push_caret = caret; caret = {};
	textf(string);
	caret = push_caret;
	clipping = push_clipping;
	return height_maximum;
}