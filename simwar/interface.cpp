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
static auto				res_units = (sprite*)gres("units", "art/objects");
static bool				can_choose_province;
static bool				element_hilited;

static bool spanel(int size) {
	rectpush push;
	width = size;
	height = texth();
	return swindow(false, 0);
}

static void texthsp(const char* string) {
	auto push_font = font;
	font = metrics::h2;
	spanel(width);
	auto push_fore = fore;
	fore = colors::h2;
	texta(string, AlignCenter);
	fore = push_fore;
	caret.y += texth() + metrics::border * 2 + metrics::padding;
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
		break;
	}
	fore = push_fore;
	fore_stroke = push_stroke;
}

//static void paint_troops_icons(const provincei* province) {
//	selector source;
//	source.querry(province);
//	if(!source)
//		return;
//	const auto dx = 16;
//	auto count = source.getcount();
//	auto push_caret = caret;
//	caret.x -= (count - 1) * dx / 2;
//	for(auto v : source) {
//		troop* p = v;
//		if(!p)
//			continue;
//		image(res_units, p->type->avatar, 0);
//		caret.x += dx;
//	}
//	caret = push_caret;
//}

void provincei::paint() const {
	if(owner)
		image(caret.x, caret.y, res_shields, owner->avatar, 0);
	if(ishilite(24)) {
		hot.cursor = cursor::Hand;
		if(hot.key == MouseLeft && hot.pressed)
			execute(cbsetptr, (long)this, 0, &game.province);
	}
	//paint_troops_icons(this);
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
	caret.y -= texth() / 2;
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

static void description_text_raw() {
	if(!dialog_description)
		return;
	textf(dialog_description);
}

static void description_text() {
	if(!dialog_description)
		return;
	height = textfs(dialog_description);
	swindow(false, 0);
	description_text_raw();
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

static void texth3a(const char* string, unsigned flags) {
	auto push_font = font;
	auto push_height = height;
	font = metrics::h3;
	texta(string, flags);
	height = push_height;
	font = push_font;
	caret.y += height_maximum;
}

static void image_block(const char* image, const char* title, fnevent right) {
	auto ps = gres(image, "art/images");
	if(!ps)
		return;
	auto push_width = width;
	auto push_caret = caret;
	auto height = ps->get(0).sy;
	draw::height = height;
	swindow(false, 0);
	draw::image(ps, 0, 0);
	caret.x += ps->get(0).sx + metrics::padding * 2 + metrics::border;
	width = width - ps->get(0).sx - metrics::border - metrics::padding * 2;
	if(title)
		texth3a(title, AlignCenter);
	if(right)
		right();
	caret = push_caret;
	width = push_width;
	caret.y += height + metrics::border * 2 + metrics::padding;
}

static void button_flat(const char* string, fnevent proc, long param = 0) {
	fire(button(string, 0, buttonfd), proc, param);
}

static void answers_block() {
	if(dialog_answers) {
		for(auto& e : *dialog_answers)
			fire(button(e.text, 0, buttonfd), buttonparam, e.id);
	}
}

static void choose_action_dialog() {
	setposru();
	if(dialog_title)
		texthsp(dialog_title);
	if(dialog_image) {
		image_block(dialog_image, 0, description_text_raw);
		caret.y += height_maximum + metrics::border * 2 + metrics::padding;
	}
	answers_block();
}

static void choose_answers_dialog() {
	width = 500;
	caret.x = (getwidth() - width) / 2;
	caret.y = 30;
	if(dialog_title)
		texthsp(dialog_title);
	height = 300;
	group(picture_image, static_text);
	caret.y += height + metrics::padding + metrics::border * 2;
	answers_block();
}

long draw::dialog(answers& an, const char* title, const char* description) {
	dialog_image = "dwarf";
	dialog_answers = &an;
	dialog_title = title;
	dialog_description = description;
	return scene(choose_answers_dialog);
}

static void progressbar(int minimal, int maximum, int current) {
	rectpush push;
	element_hilited = ishilite();
	auto push_fore = fore;
	fore = colors::red;
	width = width * (current - minimal) / (maximum - minimal);
	auto push_alpha = alpha;
	alpha = 64;
	rectf();
	alpha = push_alpha;
	width = push.width;
	fore = colors::border;
	rectb();
	fore = push_fore;
}

static void progress(const char* string, int minimal, int maximum, int current, const char* tips = 0) {
	auto push_fore_stroke = fore_stroke;
	auto push_fore = fore;
	auto push_font = font;
	font = metrics::h2;
	height = texth();
	progressbar(minimal, maximum, current);
	if(tips && element_hilited)
		get_info(tips);
	texta(string, AlignCenterCenter);
	font = push_font;
	fore = push_fore;
	fore_stroke = push_fore_stroke;
	caret.y += height;
}

static void set_default_view() {
}

static void province_info() {
	char temp[260]; stringbuilder sb(temp);
	sb.clear(); sb.add("%Explored %1i%%", game.province->get(Explored));
	progress(temp, 0, 100, game.province->get(Explored), "Explored");
	auto population = game.province->get(Population);
	auto pp = populationi::findbypopulation(population);
	auto range = pp->getrange();
	progress(getnm(pp->id), range.min, range.max, population, "Population");
}

static void province_header() {
	char temp[260]; stringbuilder sb(temp);
	sb.add(getnm(game.province->id));
	texthsp(temp);
}

static void province_info_window() {
	setposru();
	//char temp[260]; stringbuilder sb(temp);
	//sb.add("%Province %1", getnm(game.province->id));
	province_header();
	auto push_padding = metrics::padding;
	metrics::padding = 0;
	image_block(game.province->landscape->id, 0, province_info);
	//image_block("killburn", "Lord Killburn", 0);
	for(auto& e : bsdata<actioni>()) {
		if(e.is(UseOnProvince))
			button_flat(getnm(e.id), set_default_view);
	}
	button_flat(getnm("Continue"), set_default_view);
	metrics::padding = push_padding;
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
	pwindow = province_info_window;
	scene();
}