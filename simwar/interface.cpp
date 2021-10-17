#include "draw.h"
#include "draw_simple.h"
#include "main.h"

using namespace draw;

static fnevent		standart_beforemodal;
static fnevent		standart_background;

static void add_status(const char* id, int value) {
	char temp[512]; stringbuilder sb(temp);
	sb.add("%1: %2i", id, value);
	auto push_fore = fore;
	auto push_caret = caret;
	fore = colors::text;
	swindow(false);
	textf(temp);
	fore = push_fore;
	caret = push_caret;
	caret.x += width + 8 * 2;
}

static void show_status_panel() {
	auto player = playeri::getcurrent();
	if(!player)
		return;
	static variant cost[] = {Gold, Mana, Fame};
	auto push_caret = caret;
	auto push_width = width;
	auto push_height = height;
	width = 100;
	height = texth();
	caret.x = 8;
	caret.y = getheight() - height - 8;
	for(auto v : cost)
		add_status(v.getname(), player->get(v));
	caret = push_caret;
	width = push_width;
	height = push_height;
}

static void event_ismodal() {
	if(standart_beforemodal)
		standart_beforemodal();
}

static void event_background() {
	if(standart_background)
		standart_background();
	show_status_panel();
}

void draw::initialize() {
	initialize("Simwar game");
	simpleinitialize();
	image_url = "silentseas";
	standart_beforemodal = pbeforemodal;
	pbeforemodal = event_ismodal;
	standart_background = pbackground;
	pbackground = event_background;
}