#include "draw.h"
#include "draw_fix.h"
#include "draw_background.h"
#include "draw_simple.h"
#include "main.h"

using namespace draw;

fnevent	draw::moveaction;
static int show_grid = 0;
static indext goal_index = Blocked;
static indext hilite_index;
static indext hilite_route_target;
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

void draw::waitanimation() {
	uieffect::waitall();
	uieffect::clearall();
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
}

void creature::move(point pt) {
	auto p = uieffect::add();
	p->setlinked(getreference());
	p->setgoal(pt);
	p->setduration(300);
	p->wait();
}

static void paintcreatures() {
	auto push_caret = caret;
	for(auto& e : bsdata<creature>())
		e.paint();
	caret = push_caret;
}

static void paintmovement() {
	char temp[32]; stringbuilder sb(temp);
	auto push_alpha = alpha;
	auto push_fore = fore;
	auto push_caret = caret;
	for(indext i = 0; i < mpx * mpy; i++) {
		if(costmap[i] >= CostPassable)
			continue;
		if(ishilite(16)) {
			hilite_index = i;
			hot.cursor = cursor::Hand;
			if(hot.key == MouseLeft && !hot.pressed) {
				if(moveaction)
					execute(moveaction);
			}
		}
		caret = m2s(gx(i), gy(i)) - camera;
		alpha = 64;
		fore = colors::active;
		circlef(16);
	}
	caret = push_caret;
	fore = push_fore;
	alpha = push_alpha;
}

static void toggle_block() {
	if(hilite_index == Blocked)
		return;
	if(blocks[hilite_index] != Inpassable)
		blocks[hilite_index] = Inpassable;
	else
		blocks[hilite_index] = Passable;
}

static void paintblocks() {
	auto push_fore = fore;
	auto push_caret = caret;
	for(indext i = 0; i < mpx * mpy; i++) {
		caret = m2s(gx(i), gy(i)) - camera;
		if(ishilite(16)) {
			hilite_index = i;
			hot.cursor = cursor::Hand;
		}
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
			if((hot.key == MouseLeft && !hot.pressed) || (hot.key == KeySpace))
				execute(toggle_block);
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
}

static void paintall() {
	paintcommon();
	paintmovement();
	paintroute();
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

static void scene_choose_race() {
	variantlist source;
	source.select(bsdata<racei>::source);
	source.match(Race, "parent", variant(), true);
	source.choose("Choose race", "Cancel", true, "generate");
}

void draw::refreshmodal() {
	breakmodal(0);
}

void draw::modalscene(fnevent paint_proc, fnevent proc) {
	auto push_paint = draw::pbackground;
	draw::pbackground = paint_proc;
	do proc(); while(getresult() && !isnext());
	draw::pbackground = push_paint;
}

void draw::initialize() {
	metrics::padding = 4;
	metrics::border = 6;
	draw::initialize("DnD adventures");
	draw::grid_size = 70;
	draw::background::url = "hills";
	draw::pbeforemodal = beforemodalall;
	draw::pbackground = paintall;
	draw::ptips = tipsall;
}