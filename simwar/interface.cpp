#include "draw.h"
#include "draw_simple.h"
#include "main.h"

using namespace draw;

extern stringbuilder	tooltips_sb;
const char*				dialog_image;
static answers*			dialog_answers;
static const char*		dialog_title;
static const char*		dialog_description;
bool					draw::can_choose_province = true;
static auto				res_shields = (sprite*)gres("shields", "art/objects");
static auto				res_units = (sprite*)gres("units", "art/objects");
extern command*			text_formats;

static bool spanel(int size) {
	rectpush push;
	width = size;
	height = texth();
	return swindow(false);
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

static void add_status(const char* id, int value) {
	char temp[512]; stringbuilder sb(temp);
	if(equal(id, "Date"))
		game.getdate(sb);
	else {
		variant v = id;
		if(v.type==Cost)
			sb.add(":%1i:%2i", v.value, value);
		else
			sb.add("%1: %2i", getnm(id), value);
	}
	textfs(temp);
	auto hilite = spanel(width);
	auto push_caret = caret;
	textf(temp);
	caret = push_caret;
	caret.x += width + metrics::padding + metrics::border * 2;
	if(hilite)
		game.getinfo(tooltips_sb, id);
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

static void tooltipshilite() {
	if(hilite_object) {
		if(tooltips_sb.begin()[0])
			return;
		variant v = hilite_object;
		v.getinfo(tooltips_sb);
	}
}

static fnevent def_ptips;
static void main_ptips() {
	tooltipshilite();
	if(def_ptips)
		def_ptips();
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
	setposru();
}

static void static_text() {
	swindow(false);
	if(dialog_description)
		textf(dialog_description);
}

static void textfw(const char* p) {
	if(!p || p[0]==0)
		return;
	auto push_width = width;
	textfs(p);
	width = push_width;
	swindow(false);
	link[0] = 0;
	textf(p);
	if(link[0])
		game.getinfo(tooltips_sb, link);
	caret.y += metrics::border * 2;
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
	caret.y += height;
}

static void texth2a(const char* string, unsigned flags) {
	auto push_fore = fore;
	auto push_font = font;
	auto push_height = height;
	fore = colors::h2;
	font = metrics::h2;
	texta(string, flags);
	height = push_height;
	font = push_font;
	fore = push_fore;
	caret.y += height;
}

static void group(const sprite* ps, const char* tips, bool right_align = false) {
	auto push_width = width;
	auto push_fore = fore;
	fore = colors::border;
	width = ps->get(0).sx;
	height = ps->get(0).sy;
	if(right_align) {
		auto push_caret = caret;
		caret.x = caret.x + push_width - width;
		image(ps, 0, 0);
		rectb();
		control_hilited = ishilite();
		caret = push_caret;
	}
	else {
		image(ps, 0, 0);
		rectb();
		control_hilited = ishilite();
	}
	if(tips && control_hilited)
		game.getinfo(tooltips_sb, tips);
	if(right_align)
		push_width -= width + metrics::border;
	else {
		caret.x += width + metrics::border;
		push_width -= width + metrics::border;
	}
	fore = push_fore;
	width = push_width;
}

static void group(bool hilite, const char* left, const char* left_tips, fnevent middle, const char* right, const char* right_tips) {
	auto p1 = gres(left, "art/images");
	if(!p1)
		return;
	auto push_height = height;
	auto push_width = width;
	auto push_caret = caret;
	height = p1->get(0).sy;
	swindow(hilite);
	group(p1, left_tips);
	auto p2 = gres(right, "art/images");
	if(p2)
		group(p2, right_tips, true);
	auto push_h1 = height;
	if(middle)
		middle();
	caret = push_caret;
	width = push_width;
	caret.y += push_h1 + metrics::border * 2 + metrics::padding;
	height = push_height;
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

static void choose_answers_dialog() {
	width = 500;
	caret.x = (getwidth() - width) / 2;
	caret.y = 30;
	if(dialog_title)
		texth2w(dialog_title);
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
	control_hilited = ishilite();
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
	if(tips && control_hilited)
		game.getinfo(tooltips_sb, tips);
	texta(string, AlignCenterCenter);
	font = push_font;
	fore = push_fore;
	fore_stroke = push_fore_stroke;
}

static void progress_format() {
	auto string = (const char*)text_params[0];
	auto minimal = text_params[1];
	auto maximum = text_params[2];
	auto current = text_params[3];
	auto tips = (const char*)text_params[4];
	progress(string, minimal, maximum, current, tips);
}

static void province_header() {
	char temp[260]; stringbuilder sb(temp);
	sb.add(getnm(game.province->id));
	texth2w(temp);
}

static void destroy_province() {
}

static void action(action_s v, fnevent proc) {
	auto p = bsdata<actioni>::elements[v].id;
	button_flat(getnm(p), proc);
}

static void buildings_info() {
	char temp[4096]; temp[0] = 0; stringbuilder sb(temp);
	game.province->getbuildings(0, &sb);
	textfw(temp);
}

static void buildings_info_window() {
	province_header();
	pushvalue<int> push(metrics::padding, 0);
	buildings_info();
	answers an; game.province->canbuild(an);
	if(an)
		action(BuildProvince, game.build);
	if(game.province->getbuildcount())
		action(DestroyProvince, game.demontage);
	button_flat(getnm("Cancel"), buttoncancel);
}

static void choose_building() {
	scene(buildings_info_window);
}

static void button_buildings() {
	char temp[260]; stringbuilder sb(temp);
	sb.add(getnm("Buildings"));
	auto count = game.province->getbuildcount();
	if(count)
		sb.adds("(%Builded %1i)", count);
	button_flat(temp, choose_building);
}

static void province_info() {
	char temp[4096]; stringbuilder sb(temp);
	game.province->getpresent(sb);
	textfw(temp);
}

static void province_info_window() {
	province_header();
	auto push_padding = metrics::padding;
	metrics::padding = 0;
	province_info();
	button_buildings();
	button_flat(getnm("Locations"), buttoncancel);
	button_flat(getnm("Cancel"), buttoncancel);
	metrics::padding = push_padding;
}

void gamei::choose_province_action() {
	scene(province_info_window);
}

void provincei::paint() const {
	if(owner)
		image(caret.x, caret.y, res_shields, owner->avatar, 0);
	if(can_choose_province && ishilite(24)) {
		hot.cursor = cursor::Hand;
		if(hot.key == MouseLeft && hot.pressed)
			execute(cbsetptr, (long)this, 0, &game.province);
	}
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

void draw::initialize() {
	static command text_formats_commads[] = {
		{"progress", progress_format},
		{},
	};
	simpleinitialize();
	initialize("Simwar game");
	metrics::padding = 3;
	metrics::border = 5;
	image_url = "silentseas";
	// Overlaod controls
	def_beforemodal = pbeforemodal;
	pbeforemodal = main_beforemodal;
	pbackground = main_background;
	def_ptips = ptips;
	ptips = main_ptips;
	text_formats = text_formats_commads;
}