#include "draw.h"
#include "main.h"

using namespace draw;

static void beforemodalall() {
}

static void paintform() {
	auto push_fore = fore;
	fore = colors::form;
	rectf();
	fore = push_fore;
}

static void paintbackground() {
	paintform();
}

static void tipsall() {
}

void draw::initialize() {
	metrics::padding = 4;
	metrics::border = 6;
	draw::initialize("DnD adventures");
	draw::pbeforemodal = beforemodalall;
	draw::pbackground = paintbackground;
	draw::ptips = tipsall;
}

void draw::print(const char* format) {
	textf(format);
}

static void paintinfo() {
	auto push_width = width;
	auto push_caret = caret;
	setposru();
	mission.paint();
	width = push_width;
	caret = push_caret;
}

static void scene_mission() {
	paintinfo();
}

void draw::runmission() {
	scene(scene_mission);
}
