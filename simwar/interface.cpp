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
	spanel(width);
	auto push_fore = fore;
	fore = colors::text;
	texta(string, AlignCenter);
	push_fore = fore;
	caret.y += 4 * 2;
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
	caret.x += width_maximum + 4 * 3;
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

static void main_window() {
	width = 500;
	caret.x = (getwidth() - width)/2;
	caret.y = 100;
	texthead("Random Event");
}

void draw::initialize() {
	initialize("Simwar game");
	simpleinitialize();
	image_url = "silentseas";
	// Overlaod controls
	def_beforemodal = pbeforemodal;
	pbeforemodal = main_beforemodal;
	def_background = pbackground;
	pbackground = main_background;
	pwindow = main_window;
}