#include "bsreq.h"
#include "log.h"
#include "main.h"

void set_dark_theme();
void initialize_png();

static void test_ability() {
	creature source = {};
	source.create(bsdata<racei>::get(1), bsdata<classi>::get(1), Male);
}

int main(int argc, char* argv[]) {
	initialize_translation("ru");
	check_translation();
	bsreq::read("rules/basic.txt");
	if(log::geterrors())
		return -1;
#ifdef _DEBUG
	test_ability();
#endif
	initialize_png();
	set_dark_theme();
	draw::startmenu();
	return 0;
}

int _stdcall WinMain(void* ci, void* pi, char* cmd, int sw) {
	return main(0, 0);
}