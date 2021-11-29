#include "draw.h"
#include "draw_fix.h"
#include "draw_background.h"
#include "draw_simple.h"
#include "main.h"

using namespace draw;

static int show_grid = 0;
long distance(point p1, point p2);

point draw::m2s(int x, int y) {
	return {(short)(x * draw::grid_size + draw::grid_size / 2), (short)(y * draw::grid_size + draw::grid_size / 2)};
}

void draw::waitanimation() {
	uieffect::waitall();
	uieffect::clearall();
}

void creature::fixdamage(int value) const {
	auto p = uieffect::add(getposition(), "%1i", colors::red, 500);
	p->setvalue(value);
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

void creature::paint() const {
	caret = getposition() - draw::camera;
	if(avatar[0])
		imager(caret.x, caret.y, gres(avatar, "art/portraits"), 0, 32);
}

static void paintcreatures() {
	auto push_caret = caret;
	for(auto& e : bsdata<creature>())
		e.paint();
	caret = push_caret;
}

static void beforemodalall() {
	draw::simpleui::beforemodal();
}

static void paintall() {
	draw::simpleui::paint();
	draw::background::paint();
	if(show_grid)
		draw::grid();
	if(hot.key == (Ctrl + 'G'))
		execute(cbsetint, show_grid ? 0 : 1, 0, &show_grid);
	paintcreatures();
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

static void scene_combat() {
	bsdata<creature>::get(0).fight();
}

static void test_characters() {
	auto p = bsdata<creature>::add();
	p->create(bsdata<racei>::get(1), bsdata<classi>::get(1), Male);
	p->setavatar("gordek");
	p->setposition(draw::m2s(10, 5));
	auto p2 = bsdata<creature>::add();
	p2->create(bsdata<racei>::get(0), bsdata<classi>::get(1), Male);
	p2->setavatar("skeleton");
	p2->setposition(draw::m2s(11, 6));
}

void draw::startmenu() {
	metrics::padding = 4;
	metrics::border = 6;
	draw::initialize("DnD adventures");
	draw::grid_size = 70;
	draw::background::url = "hills";
	draw::pbeforemodal = beforemodalall;
	draw::pbackground = paintall;
	draw::ptips = tipsall;
	draw::setnext(scene_combat);
	//
	test_characters();
	//
	draw::start();
}