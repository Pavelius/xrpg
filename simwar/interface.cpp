#include "command.h"
#include "draw.h"
#include "draw_background.h"
#include "draw_commands.h"
#include "draw_simple.h"
#include "main.h"

using namespace draw;

const char*				dialog_image;
static answers*			dialog_answers;
static const char*		dialog_title;
static const char*		dialog_description;
static bool				can_choose_province = true;
static bool				answers_test_tips;
static const int		cell_padding = 2;
static auto				res_shields = (sprite*)gres("shields", "art/objects");
static auto				res_units = (sprite*)gres("units", "art/objects");
extern command*			text_formats;
static fnevent			active_window;

void set_dark_theme();

static void painthilite() {
	auto push_fore = fore;
	auto push_alpha = alpha;
	fore = colors::button;
	if(!hot.pressed)
		alpha = 128;
	else
		alpha = 96;
	rectf();
	alpha = push_alpha;
	//rectb();
	fore = push_fore;
}

static void paintcurrenthilite() {
	auto push_fore = fore;
	auto push_alpha = alpha;
	rectpush push;
	fore = colors::button;
	alpha = 128;
	width = hot.hilite.width();
	height = hot.hilite.height();
	caret.x = hot.hilite.x1;
	caret.y = hot.hilite.y1;
	rectb();
	alpha = push_alpha;
	fore = push_fore;
}

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
		if(v.type == Cost)
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
		game.getinfo(tips_sb, id);
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

static void tips_validate() {
	variant v;
	if(tips_sb)
		v = tips_sb.begin();
	if(!v && hilite_object && !tips_sb)
		v = hilite_object;
	if(v) {
		tips_sb.clear();
		v.getinfo(tips_sb);
	}
}

static void paint_troops(const provincei* province) {
	auto push_stroke = fore_stroke;
	auto push_fore = fore;
	auto push_font = font;
	font = metrics::small;
	fore = colors::black;
	fore_stroke = colors::white;
	for(auto& e : province->garnison) {
		textjc(getnm(e.type->id));
		caret.y += texth();
		break;
	}
	font = push_font;
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

static void static_text() {
	swindow(false);
	if(dialog_description)
		textf(dialog_description);
}

static void textfw(const char* p) {
	if(!p || p[0] == 0)
		return;
	auto push_width = width;
	textfs(p);
	width = push_width;
	swindow(false);
	textf(p);
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

static void textfc(const char* string) {
	auto push_width = width;
	auto push_caret = caret;
	textfs(string);
	if(clipping) {
		caret.x = aligned(caret.x, push_width, AlignCenterCenter, width);
		textf(string);
	}
	caret = push_caret;
	width = push_width;
}

static void textfce(const char* string) {
	auto push_width = width;
	auto push_caret = caret;
	textfs(string);
	if(clipping) {
		caret.x = caret.x - width / 2;
		textf(string);
	}
	caret = push_caret;
	width = push_width;
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
	} else {
		image(ps, 0, 0);
		rectb();
		control_hilited = ishilite();
	}
	if(tips && control_hilited)
		game.getinfo(tips_sb, tips);
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

static void paint(cost_s n) {
	switch(n) {
	case Gold:
		image(caret.x + 1, caret.y + 2, metrics::icons, n, 0);
		break;
	default:
		image(caret.x, caret.y - 1, metrics::icons, n, 0);
		break;
	}
}

static void status(cost_s n, int value) {
	auto id = bsdata<costi>::elements[n].id;
	if(ishilite())
		tips_sb.add(id);
	paint(n);
	auto push_caret = caret;;
	caret.x += 14;
	char temp[260]; stringbuilder sb(temp);
	sb.add("%+1i", value); text(temp);
	caret = push_caret;
}

static void income_format() {
	auto push_width = width;
	width = 42;
	height = texth();
	auto push_caret = caret;
	auto gold = text_params[0];
	auto mana = text_params[1];
	status(Gold, gold);
	caret.x = push_caret.x + push_width - width;
	status(Mana, mana);
	caret = push_caret;
}

static void progressbar(int minimal, int maximum, int current) {
	rectpush push;
	control_hilited = ishilite();
	auto push_fore = fore;
	fore = colors::green;
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
	height = texth() + 1 * 2;
	progressbar(minimal, maximum, current);
	if(tips && control_hilited)
		game.getinfo(tips_sb, tips);
	texta(string, AlignCenterCenter);
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

static void choose_province_action() {
	game.province = (provincei*)hot.param;
	setlastactive();
}

static void special_cicle(int ox, int oy, int value, color c1) {
	if(value <= 0)
		return;
	char temp[32]; stringbuilder sb(temp);
	sb.add("%1i", value);
	auto push_caret = caret;
	auto push_fore = fore;
	auto push_alpha = alpha;
	auto push_width = width;
	caret.x += ox; caret.y += oy;
	fore = c1;
	alpha = 196;
	circlef(8);
	alpha = push_alpha;
	fore = fore.mix(colors::form, 128);
	circle(8);
	fore = colors::black;
	caret.x -= (textw(temp) + 1) / 2;
	caret.y -= texth() / 2;
	text(temp);
	width = push_width;
	fore = push_fore;
	caret = push_caret;
}

void costa::paint() const {
	special_cicle(-27, 0, get(Gold), colors::yellow);
	special_cicle(27, 0, get(Mana), color(128, 128, 255));
}

static void paint_cost(const costa& cost) {
	auto push_caret = caret;
	auto push_fore = fore;
	auto push_stroke = fore_stroke;
	fore = colors::black;
	caret.y -= texth();
	cost.paint();
	caret = push_caret;
	fore = push_fore;
}

static void paint_neightboard(const provincei* p) {
	auto push_fore = fore;
	fore = colors::red;
	for(auto& v : p->neightboards) {
		auto p1 = (provincei*)v;
		if(!p1)
			continue;
		auto push_caret = caret;
		line(p1->position.x - camera.x, p1->position.y - camera.y);
		caret = push_caret;
	}
	fore = push_fore;
}

void provincei::paint() const {
	if(player)
		image(caret.x, caret.y, res_shields, player->avatar, 0);
	if(can_choose_province && ishilite(24)) {
		hot.cursor = cursor::Hand;
		if(hot.key == MouseLeft && hot.pressed)
			execute(choose_province_action, (long)this, 0, &game.province);
	}
	if(game.province == this)
		paint_neightboard(this);
	paint_cost(income);
	auto push_font = font;
	font = metrics::h2;
	auto push_stroke = fore_stroke;
	auto push_fore = fore;
	fore = colors::black;
	fore_stroke = colors::white;
	textjc(getnm(id));
	caret.y += texth() / 2 + 2;
	fore = push_fore;
	fore_stroke = push_stroke;
	font = push_font;
	paint_troops(this);
}

static void paintborder() {
	auto push_fore = fore;
	fore = colors::border;
	rectb();
	fore = push_fore;
}

static bool fieldex(uniti* p, playeri* player, int count, bool disabled) {
	rectpush push;
	char temp[260]; temp[0] = 0;
	stringbuilder sb(temp);
	auto hilited = ishilite();
	if(hilited) {
		if(!disabled)
			painthilite();
		hilite_object = p;
	}
	setoffset(cell_padding, cell_padding);
	auto push_fore = fore;
	if(disabled)
		fore = fore.mix(colors::form);
	text(getnm(p->id));
	if(player) {
		costa cost = p->cost;
		if(player)
			cost.modify(player->resources, p->need);
		cost.getinfo(sb, 0);
	} else if(count)
		sb.addcount("Squad", count, "%1i %-2");
	else
		sb.add("%Level %1i", p->get(Level));
	if(temp[0]) {
		auto push_width = width;
		textfs(temp);
		caret.x = caret.x + push_width - width;
		textf(temp);
	}
	fore = push_fore;
	auto result = false;
	if(hot.key == MouseLeft && !hot.pressed && hilited && !disabled)
		result = true;
	return result;
}

static void field(army& source, playeri* player, fnevent proc, army* garnison) {
	for(auto& e : source) {
		if(!e)
			continue;
		auto allow = !garnison || player->hire(e.type, garnison, false, 0);
		if(fieldex(e.type, player, 0, !allow)) {
			game.unit = &e;
			game.garnison = garnison;
			execute(proc);
		}
		caret.y += height + metrics::padding;
	}
}

bool army::choose(const char* title, const char* t1, army& a1, fnevent pr1, const char* t2, army& a2, fnevent pr2) {
	while(ismodal()) {
		paintstart();
		width = 500;
		caret.x = draw::getwidth() - width - metrics::border - metrics::padding;
		caret.y = metrics::padding + metrics::border;
		if(title)
			texth2w(title);
		height = (metrics::font->height + cell_padding * 2) * 18;
		swindow(false);
		auto push_width = width;
		auto push_height = height;
		auto push_caret = caret;
		width = 220;
		height = texth() + cell_padding * 2;
		texth3a(t1, AlignCenter);
		caret.x += metrics::border * 2;
		field(a1, game.player, pr1, &a2);
		caret = push_caret;
		caret.x = push_caret.x + push_width - width - metrics::border * 4;
		texth3a(t2, AlignCenter);
		caret.x += metrics::border * 2;
		field(a2, 0, pr2, 0);
		width = 100;
		caret = push_caret;
		caret.y += push_height + metrics::border * 2;
		fire(button(getnm("Cancel"), KeyEscape, buttonrd, false), setdefactive);
		width = push_width;
		domodal();
	}
	return getresult() != 0;
}

void draw::choose(answers& an, const char* title, const char* header) {
	auto proc = (fnevent)an.choose(title, 0, true, 0, 1, header);
	setactive(proc);
}

long draw::choosel(answers& an, const char* title, const char* header) {
	auto push = can_choose_province;
	can_choose_province = false;
	auto result = an.choose(title, getnm("Cancel"), true, 0, 1, header);
	can_choose_province = push;
	return result;
}

void draw::setactive(fnevent proc) {
	if(proc && !isnext()) {
		active_window = proc;
		setnext(proc);
	}
}

bool draw::confirm(const char* title, const char* format) {
	answers an;
	an.add(1, getnm("Yes"));
	an.add(0, getnm("No"));
	pushvalue<bool> push(can_choose_province, false);
	return an.choose(format, 0, true, 0, 2, title) != 0;
}

void draw::setlastactive() {
	setactive(active_window);
}

void draw::setdefactive() {
	setactive(game.playermove);
}

static void answers_beforepaint() {
	answers_test_tips = (tips_sb.begin()[0] == 0);
}

static void answers_afterpaint() {
	if(answers_test_tips && (hilite_object || tips_sb.begin()[0])) {
		auto push_width = width;
		auto push_height = height;
		auto push_caret = caret;
		setposru();
		width = 320;
		tips_validate();
		textfs(tips_sb);
		tips_size.x = width;
		tips_size.y = height;
		tips_caret.x = caret.x - tips_size.x - metrics::padding - metrics::border * 2;
		tips_caret.y = caret.y;
		if(hot.hilite)
			tips_caret.y = hot.hilite.y1;
		caret = push_caret;
		height = push_height;
		width = push_width;
	}
}

static void main_beforemodal() {
	simpleui::beforemodal();
}

static void main_background() {
	simpleui::paint();
	background::paint();
	paintprovinces();
	commands::paint();
	show_status_panel();
	setposru();
}

static void main_ptips() {
	tips_validate();
	if(tips_sb && hot.hilite)
		paintcurrenthilite();
	background::tips();
	simpleui::tips();
}

void draw::initialize() {
	static command text_formats_commads[] = {
		{"progress", progress_format},
		{"income", income_format},
		{},
	};
	set_dark_theme();
	initialize("Simwar game");
	metrics::padding = 3;
	metrics::border = 5;
	image_url = "silentseas";
	// Overlaod controls
	pbeforemodal = main_beforemodal;
	pbackground = main_background;
	ptips = main_ptips;
	text_formats = text_formats_commads;
	answers::beforepaint = answers_beforepaint;
	answers::afterpaint = answers_afterpaint;
}