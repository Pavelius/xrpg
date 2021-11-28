#include "bsreq.h"
#include "draw.h"
#include "draw_simple.h"
#include "log.h"
#include "main.h"

void set_dark_theme();
void initialize_png();

static void scene_choose_race() {
	answers an;
	for(auto& e : bsdata<racei>())
		an.add((long)&e, e.id);
	an.choose("Choose **race**", "Cancel", true, 0);
}

int main(int argc, char* argv[]) {
	bsreq::read("rules/basic.txt");
	if(log::geterrors())
		return -1;
	initialize_png();
	set_dark_theme();
	metrics::padding = 4;
	metrics::border = 4;
	draw::initialize("DnD adventures");
	draw::pbeforemodal = draw::simpleui::beforemodal;
	draw::pbackground = draw::simpleui::paint;
	draw::ptips = draw::simpleui::tips;
	draw::setnext(scene_choose_race);
	draw::start();
	return 0;
}

int _stdcall WinMain(void* ci, void* pi, char* cmd, int sw) {
	return main(0, 0);
}