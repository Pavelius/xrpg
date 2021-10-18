#include "draw.h"
#include "draw_simple.h"
#include "main.h"

using namespace draw;

extern stringbuilder	tooltips_sb;
const char*				dialog_image;
static answers*			dialog_answers;
static const char*		dialog_title;
static const char*		dialog_description;

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
	fore = colors::h2;
	texta(string, AlignCenter);
	fore = push_fore;
	font = push_font;
}

static void picture_image() {
	rectpush push;
	setoffset(-metrics::border, -metrics::border);
	auto ps = gres(dialog_image, "art/images");
	if(ps)
		image(caret.x, caret.y, ps, 0, 0);
	auto push_fore = fore;
	fore = colors::border;
	rectb();
	fore = push_fore;
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
	textf(temp);
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

static void static_text() {
	swindow(false, 0);
	if(dialog_description)
		textf(dialog_description);
}

static void group(fnevent left, fnevent right) {
	rectpush push;
	auto x2 = caret.x + width;
	auto sp = metrics::padding + metrics::border * 2;
	width = 160;
	left();
	caret.x += width + sp;
	width = x2 - caret.x;
	right();
}

static void choose_answers_dialog() {
	width = 500;
	caret.x = (getwidth() - width)/2;
	caret.y = 30;
	if(dialog_title) {
		texthead(dialog_title);
		caret.y += height_maximum + metrics::padding + metrics::border * 2;
	}
	height = 300;
	group(picture_image, static_text);
	caret.y += height + metrics::padding + metrics::border * 2;
	if(dialog_answers) {
		for(auto& e : *dialog_answers) {
			if(button(e.text, 0, buttonfd)) {

			}
		}
	}
}

int draw::dialog(answers& an, const char* title, const char* description) {
	dialog_image = "dwarf";
	dialog_answers = &an;
	dialog_title = title;
	dialog_description = description;
	mainscene(choose_answers_dialog);
	return getresult();
}

void draw::initialize() {
	simpleinitialize();
	initialize("Simwar game");
	metrics::padding = 3;
	metrics::border = 5;
	image_url = "silentseas";
	// Overlaod controls
	def_beforemodal = pbeforemodal;
	pbeforemodal = main_beforemodal;
	def_background = pbackground;
	pbackground = main_background;
}