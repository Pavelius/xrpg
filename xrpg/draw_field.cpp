#include "crt.h"
#include "command.h"
#include "draw.h"
#include "draw_button.h"
#include "draw_clipboard.h"
#include "draw_focus.h"
#include "draw_scroll.h"
#include "handler.h"

using namespace draw;

typedef adat<int, 32> fldstra;

int					metrics::edit = 2;
static char			current_buffer[4096];
static int			current_width, current_height;
static int			current_origin_height;
static int			current_origin_cashe;
static int			current_maximum_height;
static bool			current_isnumber;
static int			current_size;
static void*		current_source;
static const char*	current_cashe;
static int			i1, i2;

static int texth(const char* string, int width, int height_origin, const char** current_cashe) {
	int dy = texth();
	int y1 = 0;
	auto p = string;
	*current_cashe = 0;
	while(*p) {
		auto c = textbc(p, width);
		if(!c)
			break;
		y1 += dy;
		if(current_cashe && y1 > height_origin) {
			*current_cashe = p;
			current_cashe = 0;
		}
		p = skiptr(p + c);
	}
	return y1;
}

static void need_update() {
	current_origin_cashe = -1;
}

static const char* getsource(void* source, int size, bool istext, stringbuilder& sb) {
	if(!source)
		return "";
	if(istext) {
		if(size == sizeof(const char*)) {
			auto p = (const char*)getsource(source, size);
			if(p)
				return p;
		} else
			return (const char*)source;
	} else {
		sb.add("%1i", getsource(source, size));
		return sb.begin();
	}
	return "";
}

static void setsource(void* source, int size, bool istext, const char* result) {
	if(!source)
		return;
	long value = 0;
	if(istext) {
		if(size == sizeof(const char*))
			value = (int)szdup(result);
		else {
			int len = zlen(result);
			if(len > size - 1)
				len = size - 1;
			auto p = (char*)source;
			memcpy(p, result, len);
			p[len] = 0;
		}
	} else
		stringbuilder::read(result, value);
	setsource(source, size, value);
}

static void choosefield() {
}

static void select(int v1, int v2) {
	if(v2 == -2)
		v2 = zlen(current_buffer);
	i1 = v1; i2 = v2;
}

static void select(int v1) {
	if(i2 == -1)
		i2 = i1;
	i1 = v1;
}

static void post_select() {
	select(hot.param, hot.param2);
}

static void post_select_mouse() {
	if(hot.param2 == 1 && hot.param != i2)
		select(hot.param, (i2 == -1) ? i1 : i2);
	else
		select(hot.param, -1);
}

static bool isspace(char sym) {
	return sym == ' ' || sym == 10 || sym == 13 || sym == 9;
}

static int next_right(bool word) {
	auto n = i1 + 1;
	if(word) {
		for(; current_buffer[n] && !isspace(current_buffer[n]); n++);
		for(; current_buffer[n] && isspace(current_buffer[n]); n++);
	}
	return n;
}

static int next_left(bool word) {
	auto n = i1;
	if(word) {
		for(; n > 0 && isspace(current_buffer[n - 1]); n--);
		for(; n > 0 && !isspace(current_buffer[n - 1]); n--);
	} else
		n = n - 1;
	return n;
}

static void post_select_ex() {
	select(hot.param);
}

static void right(bool shift, bool word) {
	auto n = zlen(current_buffer);
	if(shift) {
		if(i1 < n)
			execute(post_select_ex, next_right(word), 0);
	} else if(i2 == -1) {
		if(i1 < n)
			execute(post_select, next_right(word), -1);
	} else if(i1 < i2)
		execute(post_select, i2, -1);
	else
		execute(post_select, i1, -1);
}

static void left(bool shift, bool word) {
	if(shift) {
		if(i1 > 0)
			execute(post_select_ex, next_left(word), 0);
	} else if(i2 == -1) {
		if(i1 > 0)
			execute(post_select, next_left(word), -1);
	} else if(i1 < i2)
		execute(post_select, i1, -1);
	else
		execute(post_select, i2, -1);
}

static void home(bool shift) {
	if(i1 == 0 && i2 == -1)
		return;
	if(shift)
		execute(post_select_ex, 0, 0);
	else
		execute(post_select, 0, -1);
}

static void end(bool shift) {
	auto n = zlen(current_buffer);
	if(i1 == n && i2 == -1)
		return;
	if(shift)
		execute(post_select_ex, n, 0);
	else
		execute(post_select, n, -1);
}

static bool paste(const char* string) {
	int maximum = sizeof(current_buffer);
	auto sn = zlen(string);
	auto bn = zlen(current_buffer);
	if(i1 + sn >= maximum)
		sn = maximum - bn;
	if(sn <= 0)
		return false;
	if(i1 < bn)
		memmove(current_buffer + i1 + sn, current_buffer + i1, bn - sn);
	memcpy(current_buffer + i1, string, sn);
	current_buffer[bn + sn] = 0;
	select(i1 + sn, -1);
	need_update();
	return true;
}

static void clear() {
	if(i2 == -1 || i1 == i2)
		return;
	if(i1 >= i2)
		iswap(i1, i2);
	auto bn = zlen(current_buffer);
	auto sn = i2 - i1;
	memmove(current_buffer + i1, current_buffer + i2, bn - i2);
	current_buffer[bn - sn] = 0;
	select(i1, -1);
	need_update();
}

static void post_symbol() {
	char temp[16] = {}; temp[0] = (char)hot.param;
	clear();
	paste(temp);
}

static void field_save() {
	if(!current_source)
		return;
	if(current_isnumber) {
		long result;
		stringbuilder::read(current_buffer, result);
		setsource(current_source, current_size, result);
	} else if(current_size == sizeof(const char*))
		setsource(current_source, current_size, (long)szdup(current_buffer));
	else
		zcpy((char*)current_source, current_buffer, current_size);
}

void field_save_and_clear() {
	field_save();
	current_source = 0;
	current_size = 0;
}

static void field_save_and_select() {
	field_save();
	select(0, -2);
}

static void field_read(void* source, int size, bool isnumber, int& i1, int& i2, unsigned flags, int width, int height) {
	if(current_source == source && current_size == size)
		return;
	field_save_and_clear();
	stringbuilder sb(current_buffer); current_buffer[0] = 0;
	auto value = (const char*)getsource(source, size, !isnumber, sb);
	if(value != sb.begin()) {
		sb.clear();
		sb.copy(value);
	}
	current_source = source;
	current_size = size;
	current_isnumber = isnumber;
	current_width = width;
	current_height = height;
	current_origin_height = 0;
	auto n = zlen(current_buffer);
	if(i1 > n)
		i1 = n;
	if(i2 == -2)
		i2 = n;
	else
		i2 = -1;
	need_update();
}

static void setcedit() {
	auto p = (void*)hot.object;
	auto s = hot.param2;
	auto v = hot.param;
	field_save_and_clear();
	i1 = 0; i2 = -2;
	setsource(p, s, v);
}

static bool isallow(const void* object, const char* id) {
	if(equal(id, "Cut"))
		return i2 != -1;
	if(equal(id, "Copy"))
		return i2 != -1;
	if(equal(id, "Paste"))
		return clipboard::isallowpaste();
	return false;
}

static void copy() {
	auto sn = i2 - i1 + 1;
	if(sn > 0)
		clipboard::copy(current_buffer + i1, i2 - i1);
}

static void cut() {
	copy();
	clear();
}

static void paste() {
	long size;
	auto p = clipboard::paste(&size);
	if(!p)
		return;
	clear();
	paste(p);
	delete p;
}

static void select_all() {
	select(0, -2);
}

static void post_double_click() {
	select(next_left(true), next_right(true));
}

static command field_commands[] = {
	{"Cut", cut, Ctrl + 'X'},
	{"Copy", copy, Ctrl + 'C'},
	{"Paste", paste, Ctrl + 'V'},
	{"SelectAll", select_all, Ctrl + 'A'},
};

static void field_focus(const rect& rc, void* source, int size, bool isnumber, unsigned flags) {
	int n;
	field_read(source, size, isnumber, i1, i2, flags, rc.width(), rc.height());
	if(current_origin_cashe != current_origin_height) {
		current_maximum_height = texth(current_buffer, current_width, current_origin_height, &current_cashe);
		current_origin_cashe = current_origin_height;
	}
	if(current_cashe) {
		draw::scroll scrollh(current_origin_height, rc.height(), current_maximum_height, rc, false, texth());
		scrollh.correct(); scrollh.input();
		auto n = current_cashe - current_buffer;
		texte(rc, current_cashe, flags, i1 - n, (i2==-1) ? -1 : i2 - n);
		scrollh.view(true);
	}
	switch(hot.key) {
	case KeyLeft:
	case KeyLeft | Shift:
	case KeyLeft | Ctrl:
	case KeyLeft | Shift | Ctrl:
		left((hot.key & Shift) != 0, (hot.key & Ctrl) != 0);
		break;
	case KeyRight:
	case KeyRight | Shift:
	case KeyRight | Ctrl:
	case KeyRight | Shift | Ctrl:
		right((hot.key & Shift) != 0, (hot.key & Ctrl) != 0);
		break;
	case KeyHome:
	case KeyHome | Shift:
		home((hot.key & Shift) != 0);
		break;
	case KeyEnd:
	case KeyEnd | Shift:
		end((hot.key & Shift) != 0);
		break;
	case KeyBackspace:
		if(i1 > 0 && i2 == -1)
			select(i1 - 1, i1);
		execute(clear);
		break;
	case MouseLeft:
	case MouseLeft | Shift:
		if(current_cashe && ishilite(rc)) {
			if(hot.pressed) {
				n = zlen(current_buffer);
				auto ni = hittest(rc, current_cashe, flags, hot.mouse);
				auto n2 = ((hot.key & Shift) != 0) ? 1 : -1;
				switch(ni) {
				case -1: break;
				case -2: execute(post_select_mouse, 0, n2); break;
				case -3: execute(post_select_mouse, n, n2); break;
				default: execute(post_select_mouse, ni + (current_cashe - current_buffer), n2); break;
				}
			}
		}
		break;
	case MouseRight:
		if(ishilite(rc)) {
			if(!hot.pressed) {
				static const char* commands[] = {"Cut", "Copy", "Paste", 0};
				auto id = contextmenu(commands, 0, isallow, 0);
				auto p = field_commands->find(id);
				if(p)
					execute(p->proc);
			}
		}
		break;
	case KeyEnter:
		execute(field_save_and_select);
		break;
	case MouseLeftDBL:
		if(ishilite(rc))
			execute(post_double_click);
		break;
	case KeyDelete:
		if(i2 != -1)
			execute(clear, 0, 0);
		else {
			auto n = zlen(current_buffer);
			if(i1 != n) {
				select(i1, i1 + 1);
				execute(clear, 0, 0);
			}
		}
		break;
	case InputSymbol:
		if(hot.param < 0x20)
			break;
		if(isnumber) {
			if(isnum(hot.param))
				execute(post_symbol, hot.param);
		} else
			execute(post_symbol, hot.param);
		break;
	default:
		if(true) {
			auto p = field_commands->find(hot.key);
			if(p)
				execute(p->proc);
		}
		break;
	}
}

static void fieldf(const rect& rco, unsigned flags, void* source, int size, int digits, bool increment, bool istext, fnchoose pchoose) {
	if(rco.width() <= 0)
		return;
	rect rc = rco;
	const unsigned edit_mask = AlignMask | TextSingleLine | TextBold | TextStroke;
	rectf(rc, colors::window);
	rectb(rc, colors::border);
	auto focused = isfocused(rc, source);
	if(increment) {
		auto result = addbutton(rc, focused,
			"+", Ctrl + KeyUp, getnm("Increase"),
			"-", Ctrl + KeyDown, getnm("Decrease"));
		auto value = getsource(source, size);
		switch(result) {
		case 1: execute(setcedit, value + 1, size, source); break;
		case 2: execute(setcedit, value - 1, size, source); break;
		}
	}
	if(pchoose) {
		if(addbutton(rc, focused, "...", F4, "Выбрать"))
			draw::execute(choosefield, (long)pchoose, size, source);
	}
	rc.offset(metrics::edit);
	if(focused) {
		field_focus(rc, source, size, increment, flags & edit_mask);
	} else {
		char temp[260]; stringbuilder sb(temp);
		auto p = getsource(source, size, istext, sb);
		text(rc, p, flags & edit_mask);
	}
}

void draw::fieln(int x, int& y, int width, const char* label, void* source, int size, int label_width, int digits) {
	setposition(x, y, width);
	titletext(x, y, width, label, label_width);
	rect rc = {x, y, x + width, y + draw::texth() + metrics::edit * 2};
	fieldf(rc, AlignRight | TextSingleLine, source, size, digits, true, false, 0);
	y += rc.height() + metrics::padding;
}

void draw::field(int x, int& y, int width, const char* label, char* source, unsigned size, int label_width, fnchoose choosep) {
	setposition(x, y, width);
	titletext(x, y, width, label, label_width);
	rect rc = {x, y, x + width, y + draw::texth() + metrics::edit * 2};
	fieldf(rc, AlignLeft | TextSingleLine, source, size, -1, false, true, choosep);
	y += rc.height() + metrics::padding;
}

void draw::field(int x, int& y, int width, int line_height, const char* label, char* source, unsigned size, int label_width, fnchoose choosep) {
	setposition(x, y, width);
	titletext(x, y, width, label, label_width);
	rect rc = {x, y, x + width, y + draw::texth() * line_height + metrics::edit * 2};
	fieldf(rc, AlignLeft, source, size, -1, false, true, choosep);
	y += rc.height() + metrics::padding;
}

void draw::field(int x, int& y, int width, const char* label, const char*& source, int label_width, fnchoose choosep) {
	setposition(x, y, width);
	titletext(x, y, width, label, label_width);
	rect rc = {x, y, x + width, y + draw::texth() + metrics::edit * 2};
	fieldf(rc, AlignLeft | TextSingleLine, &source, sizeof(source), -1, false, true, choosep);
	y += rc.height() + metrics::padding;
}

HANDLER(before_setfocus) {
	field_save_and_clear();
}