#include "draw.h"
#include "draw_simple.h"
#include "main.h"

using namespace draw;

extern stringbuilder tooltips_sb;

static bool spanel(int size) {
	rectpush push;
	width = size;
	height = texth();
	return swindow(false, 0);
}

static void texthead(const char* string) {
	auto push_font = font;
	font = metrics::h2;
	spanel(width);
	auto push_fore = fore;
	fore = colors::text;
	texta(string, AlignCenter);
	fore = push_fore;
	font = push_font;
}

static void picture(const char* string) {
	swindow(false, 0);
	width_maximum = width;
	height_maximum = height;
}

static void add_description(stringbuilder& sb, const char* id) {
	auto p = getdescription(id);
	if(p)
		sb.addn(p);
}

static void get_info(const char* id) {
	tooltips_sb.addn("###%1", getnm(id));
	add_description(tooltips_sb, id);
}

static void add_status(const char* id, int value) {
	char temp[512]; stringbuilder sb(temp);
	sb.add("%1: %2i", getnm(id), value);
	textfs(temp);
	auto hilite = spanel(width_maximum);
	auto push_caret = caret;
	auto push_fore = fore;
	fore = colors::text;
	textf(temp);
	fore = push_fore;
	caret = push_caret;
	caret.x += width_maximum + metrics::padding + metrics::border * 2;
	if(hilite)
		get_info(id);
}

static void show_status_panel() {
	auto player = playeri::getcurrent();
	if(!player)
		return;
	static variant cost[] = {Gold, Mana, Fame};
	setposld();
	auto push_height = height;
	height = texth();
	for(auto v : cost)
		add_status(v.getid(), player->get(v));
	height = push_height;
}

static fnevent def_beforemodal;
static void main_beforemodal() {
	if(def_beforemodal)
		def_beforemodal();
	tooltipsbeforemodal();
}

static fnevent def_background;
static void main_background() {
	if(def_background)
		def_background();
	show_status_panel();
}

static void picture_image() {
	picture("Picture");
}

static void static_text() {
	picture("Picture");
}

static void answer_button() {

}

static void group(fnevent left, fnevent right) {
	rectpush push;
	auto x2 = caret.x + width;
	auto sp = metrics::padding + metrics::border * 2;
	auto dx = (push.width - sp + 2) / 3;
	width = dx;
	left();
	caret.x += dx + sp;
	width = dx * 2;
	if(caret.x + width > x2)
		width = x2 - caret.x;
	right();
}

static void main_window() {
	width = 500;
	caret.x = (getwidth() - width)/2;
	caret.y = 30;
	texthead(getnm("RandomEvent"));
	caret.y += height_maximum + metrics::padding + metrics::border * 2;
	height = 300;
	group(picture_image, static_text);
	caret.y += height + metrics::padding + metrics::border * 2;
	auto push_fore = fore;
	fore = colors::text;
	button("Test 1", '1', buttonfd);
	button("Test 2", '2', buttonfd);
	button("Test 3", '3', buttonfd);
	button("Test 4", '4', buttonfd);
	fore = push_fore;
}

void draw::initialize() {
	initialize("Simwar game");
	simpleinitialize();
	metrics::padding = 3;
	metrics::border = 5;
	image_url = "silentseas";
	// Overlaod controls
	def_beforemodal = pbeforemodal;
	pbeforemodal = main_beforemodal;
	def_background = pbackground;
	pbackground = main_background;
	pwindow = main_window;
}