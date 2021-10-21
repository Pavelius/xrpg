#include "draw.h"
#include "draw_simple.h"
#include "main.h"

using namespace draw;

extern stringbuilder	tooltips_sb;
const char*				dialog_image;
static answers*			dialog_answers;
static const char*		dialog_title;
static const char*		dialog_description;
static auto				res_shields = (sprite*)gres("shields", "art/objects");

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

static void textblock(const char* string) {
	spanel(width);
	texta(string, AlignCenterCenter);
}

static void textjc(const char* string) {
	auto push_caret = caret;
	auto w = textw(string);
	auto h = texth();
	caret.x -= w / 2;
	caret.y -= h / 2;
	text(string, -1, TextStroke);
	caret = push_caret;
}

static void picture_image() {
	auto ps = gres(dialog_image, "art/images");
	if(!ps)
		return;
	setoffset(-metrics::border, -metrics::border);
	image(caret.x, caret.y, ps, 0, 0);
	width = ps->get(0).sx;
	height = ps->get(0).sy;
	auto push_fore = fore;
	fore = colors::border;
	rectb();
	fore = push_fore;
	setoffset(metrics::border, metrics::border);
}

static void add_description(stringbuilder& sb, const char* id) {
	auto p = getdescription(id);
	if(p) {
		if(equal(id, "Date"))
			sb.addn(p, game.getyear(), game.getturn());
		else
			sb.addn(p);
	}
}

static void get_info(const char* id) {
	tooltips_sb.addn("###%1", getnm(id));
	add_description(tooltips_sb, id);
}

static void add_status(const char* id, int value) {
	char temp[512]; stringbuilder sb(temp);
	if(equal(id, "Date"))
		game.getdate(sb);
	else
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
	auto player = game.player;
	if(!player)
		return;
	setposld();
	auto push_height = height;
	height = texth();
	add_status("Date", 0);
	for(auto& e : bsdata<costi>())
		add_status(e.id, player->get(variant(&e)));
	height = push_height;
}

static fnevent def_beforemodal;
static void main_beforemodal() {
	if(def_beforemodal)
		def_beforemodal();
	tooltipsbeforemodal();
}

static void paint_troops(const provincei* province) {
	auto push_stroke = fore_stroke;
	auto push_fore = fore;
	selector source;
	source.querry(province);
	fore = colors::black;
	fore_stroke = colors::white;
	for(auto v : source) {
		troop* p = v;
		if(!p)
			continue;
		textjc(getnm(p->type->id));
		caret.y += texth();
	}
	fore = push_fore;
	fore_stroke = push_stroke;
}

void provincei::paint() const {
	if(owner)
		image(caret.x, caret.y, res_shields, owner->avatar, 0);
	auto push_font = font;
	font = metrics::h2;
	auto push_stroke = fore_stroke;
	auto push_fore = fore;
	fore = colors::black;
	fore_stroke = colors::white;
	textjc(getnm(id));
	caret.y += texth();
	fore = push_fore;
	fore_stroke = push_stroke;
	font = push_font;
	caret.y -= texth()/2;
	paint_troops(this);
}

static void paintprovinces() {
	for(auto& e : bsdata<provincei>()) {
		set(e.position.x, e.position.y);
		if(isclipped(64))
			continue;
		e.paint();
	}
}

static void main_background() {
	paintclear();
	paintimage();
	paintprovinces();
	paintcommands();
	show_status_panel();
}

static void static_text() {
	swindow(false, 0);
	if(dialog_description)
		textf(dialog_description);
}

static void description_text() {
	if(!dialog_description)
		return;
	height = textfs(dialog_description);
	swindow(false, 0);
	textf(dialog_description);
}

static void group(fnevent left, fnevent right) {
	rectpush push;
	auto x2 = caret.x + width;
	auto sp = metrics::padding + metrics::border * 2;
	left();
	caret.x += width + sp;
	width = x2 - caret.x;
	right();
}

static void choose_action_dialog() {
	setposru();
	if(dialog_title) {
		texthead(dialog_title);
		caret.y += height_maximum + metrics::border * 2 + metrics::padding;
	}
	description_text();
	caret.y += metrics::border * 2 + metrics::padding;
	if(dialog_answers) {
		for(auto& e : *dialog_answers)
			fire(button(e.text, 0, buttonfd), buttonparam, e.id);
	}
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
		for(auto& e : *dialog_answers)
			fire(button(e.text, 0, buttonfd), buttonparam, e.id);
	}
}

long draw::dialog(answers& an, const char* title, const char* description) {
	dialog_image = "dwarf";
	dialog_answers = &an;
	dialog_title = title;
	dialog_description = description;
	return scene(choose_answers_dialog);
}

static void choose_province() {
	answers an;
	an.add(0, getnm("Continue"));
	dialog_answers = &an;
	dialog_title = getnm(game.province->id);
	dialog_description = "Test string for long visualisation model and some description can be on next line.";
	scene(choose_action_dialog);
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
	pbackground = main_background;
}

void draw::maketurn() {
	choose_province();
	game.maketurn();
	if(!isnext())
		setnext(draw::maketurn);
}