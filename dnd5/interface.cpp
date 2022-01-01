#include "draw.h"
#include "draw_fix.h"
#include "draw_background.h"
#include "draw_simple.h"
#include "main.h"

using namespace draw;

static int show_grid = 0;
static int show_movement = 0;
static indext goal_index = Blocked;
indext draw::hilite_index;
static indext hilite_route_target;
static rect hilite_index_rect;
fnevent draw::mouseaction;
fnevent draw::tipscustom;
long distance(point p1, point p2);

point draw::m2s(int x, int y) {
	return {
		(short)(x * draw::grid_size + draw::grid_size / 2),
		(short)(y * draw::grid_size + draw::grid_size / 2)
	};
}

point draw::s2m(point p) {
	return {
		(short)(p.x / draw::grid_size),
		(short)(p.y / draw::grid_size)
	};
}

static point getpos(point start, point goal, int range) {
	point result;
	//auto range = draw::grid_size / 2;
	auto lenght = distance(start, goal);
	auto dx = goal.x - start.x;
	auto dy = goal.y - start.y;
	result.x = (short)(start.x + range * dx / lenght);
	result.y = (short)(start.y + range * dy / lenght);
	return result;
}

void creature::fixmiss() {
	uieffect::add(getposition(), getnm("Miss"), colors::gray, 500);
}

void creature::fixattack(point goal, ability_s type) {
	auto start = getposition();
	auto lenght = distance(start, goal);
	if(!lenght)
		return;
	auto p = uieffect::add();
	p->setlinked(getreference());
	p->setgoal(getpos(start, goal, draw::grid_size / 2));
	p->setduration(250);
	p->wait();
	p->setgoal(start);
	p->setduration(400);
}

void creature::fixdamage(int value) const {
	auto p = uieffect::add(getposition(), "%1i", colors::red, 500);
	p->setvalue(value);
}

void creature::paint() const {
	caret = getposition() - draw::camera;
	if(avatar[0])
		imager(caret.x, caret.y, gres(avatar, "art/portraits"), 0, 32);
	ishilite(20, this);
	auto push_fore = fore;
	if(is(Hostile))
		fore = colors::red;
	else
		fore = colors::green;
	circle(32);
	fore = push_fore;
}

void creature::move(point pt) {
	auto p = uieffect::add();
	p->setlinked(getreference());
	p->setgoal(pt);
	p->setduration(200);
	waitanimation();
}

static void paintcreatures() {
	auto push_caret = caret;
	for(auto& e : bsdata<creature>())
		e.paint();
	caret = push_caret;
}

static void hilite_index_test(indext i) {
	if(ishilite(16)) {
		hilite_index = i;
		hilite_index_rect = hot.hilite;
		hot.cursor = cursor::Hand;
	}
}

static void paintmovement() {
	char temp[32]; stringbuilder sb(temp);
	auto push_alpha = alpha;
	auto push_fore = fore;
	auto push_caret = caret;
	alpha = 64;
	for(indext i = 0; i < mpx * mpy; i++) {
		if(costmap[i] >= CostPassable)
			continue;
		caret = m2s(gx(i), gy(i)) - camera;
		hilite_index_test(i);
		if(show_movement) {
			fore = colors::black;
			circlef(16);
		}
	}
	caret = push_caret;
	fore = push_fore;
	alpha = push_alpha;
}

static void paintblocks() {
	auto push_fore = fore;
	auto push_caret = caret;
	for(indext i = 0; i < mpx * mpy; i++) {
		caret = m2s(gx(i), gy(i)) - camera;
		hilite_index_test(i);
		if(isblocked(i)) {
			fore = colors::border;
			circle(16);
			auto push_alpha = alpha;
			alpha = 128;
			fore = colors::black;
			circlef(16);
			alpha = push_alpha;
		}
		if(hilite_index == i) {
			if(hot.pressed)
				fore = colors::active.mix(colors::form);
			else
				fore = colors::active;
			circle(16);
		}
	}
	caret = push_caret;
	fore = push_fore;
}

static void paintroute() {
	if(hilite_index != hilite_route_target) {
		hilite_route_target = hilite_index;
		routeto(hilite_route_target);
	}
	if(!indecies)
		return;
	auto push_fore = fore;
	auto push_caret = caret;
	auto count = 0;
	fore = colors::green;
	for(auto i : indecies) {
		auto pt = m2s(gx(i), gy(i)) - camera;
		if(count == 0) {
			caret = pt;
			circle(16);
		} else
			line(pt.x, pt.y);
		count++;
	}
	caret = push_caret;
	fore = push_fore;
}

static void beforemodalall() {
	draw::simpleui::beforemodal();
	hilite_index = Blocked;
}

static void paintcommon() {
	draw::simpleui::paint();
	draw::background::paint();
	if(show_grid)
		draw::grid();
	if(hot.key == (Ctrl + 'G'))
		execute(cbsetint, show_grid ? 0 : 1, 0, &show_grid);
	if(hot.key == (Ctrl + 'M'))
		execute(cbsetint, show_movement ? 0 : 1, 0, &show_movement);
}

static void painteffecntamove() {
	paintcommon();
	paintmovement();
	paintroute();
	paintcreatures();
}

static void painteffect() {
	paintcommon();
	paintcreatures();
}

void draw::painteditor() {
	paintcommon();
	paintblocks();
}

static void tipsall() {
	draw::simpleui::tips();
	draw::background::tips();
}

static void beforetips() {
	if(hilite_index_rect == hot.hilite && hilite_index != Blocked) {
		if(!tips_sb)
			tips_sb.add("%Cell (%1i, %2i)", gx(hilite_index), gy(hilite_index));
		if(mouseaction) {
			if((hot.key == MouseLeft && !hot.pressed) || hot.key == KeySpace)
				execute(mouseaction);
		}
	}
	if(tipscustom)
		tipscustom();
}

static void scene_choose_race() {
	variantlist source;
	source.select(bsdata<racei>::source);
	source.match(Race, "parent", variant(), true);
	source.choose("Choose race", "Cancel", true, "generate");
}

void draw::refreshmodal() {
	breakmodal(0);
}

void draw::waitanimation() {
	auto push_paint = pbackground;
	pbackground = painteffect;
	uieffect::waitall();
	uieffect::clearall();
	pbackground = push_paint;
}

void draw::modalscene(fnevent paint_proc, fnevent proc, fnevent mouse_proc) {
	auto push_paint = pbackground;
	auto push_mouse = mouseaction;
	if(paint_proc)
		pbackground = paint_proc;
	if(mouse_proc)
		mouseaction = mouse_proc;
	do proc(); while(getresult() && !isnext());
	mouseaction = push_mouse;
	pbackground = push_paint;
	hilite_route_target = Blocked;
	indecies.clear();
}

static void choose_index() {
	goal_index = hilite_route_target;
	breakmodal(1);
}

indext draw::chosemovement() {
	show_movement = true;
	auto push_proc = draw::pbackground;
	auto push_mouse = mouseaction;
	mouseaction = choose_index;
	draw::pbackground = painteffecntamove;
	draw::scene(beforetips);
	mouseaction = push_mouse;
	draw::pbackground = push_proc;
	auto r = goal_index;
	goal_index = Blocked;
	return r;
}

void draw::initialize() {
	metrics::padding = 4;
	metrics::border = 6;
	draw::initialize("DnD adventures");
	draw::grid_size = 70;
	draw::background::url = "hills";
	draw::pbeforemodal = beforemodalall;
	draw::pbackground = painteffect;
	draw::ptips = tipsall;
	answers::afterpaint = beforetips;
}