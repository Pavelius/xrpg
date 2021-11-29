#include "bsreq.h"
#include "draw.h"
#include "draw_simple.h"
#include "log.h"
#include "main.h"

void set_dark_theme();
void initialize_png();

static void scene_choose_race() {
	variantlist source;
	source.select(bsdata<racei>::source);
	source.match(Race, "parent", variant(), true);
	source.choose("Choose race", "Cancel", true, "generate");
}

static void scene_test_time() {
	ui::fixbegin();
	ui::fixtext({100, 100}, "Test string", colors::text, 500);
	ui::fixend();
	draw::setnext(scene_choose_race);
}

static void test_ability() {
	creature source = {};
	source.create(bsdata<racei>::get(1), bsdata<classi>::get(1), Male);
}

int main(int argc, char* argv[]) {
	bsreq::read("rules/basic.txt");
	if(log::geterrors())
		return -1;
#ifdef _DEBUG
	test_ability();
#endif
	initialize_png();
	set_dark_theme();
	metrics::padding = 4;
	metrics::border = 6;
	draw::initialize("DnD adventures");
	draw::pbeforemodal = draw::simpleui::beforemodal;
	draw::pbackground = draw::simpleui::paint;
	draw::ptips = draw::simpleui::tips;
	draw::setnext(scene_test_time);
	draw::start();
	return 0;
}

int _stdcall WinMain(void* ci, void* pi, char* cmd, int sw) {
	return main(0, 0);
}