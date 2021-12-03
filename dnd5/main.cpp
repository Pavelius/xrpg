#include "bsreq.h"
#include "log.h"
#include "main.h"

void set_dark_theme();
void initialize_png();

static void test_ability() {
	creature source = {};
	source.create(bsdata<racei>::get(1), bsdata<classi>::get(1), Male);
}

static void test_characters() {
	auto p = bsdata<creature>::add();
	p->create(bsdata<racei>::get(1), bsdata<classi>::get(1), Male);
	p->setavatar("gordek");
	p->setposition(draw::m2s(14, 9));
	auto p2 = bsdata<creature>::add();
	p2->create(bsdata<racei>::get(0), bsdata<classi>::get(1), Male);
	p2->setavatar("skeleton");
	p2->setposition(draw::m2s(11, 6));
	p2->set(Hostile);
}

int main(int argc, char* argv[]) {
	initialize_translation("ru");
	check_translation();
	bsreq::read("rules/basic.txt");
	if(log::geterrors())
		return -1;
	initialize_png();
	set_dark_theme();
	draw::initialize();
#ifdef _DEBUG
	test_ability();
	test_characters();
#endif
	draw::setnext(gamei::runeditor);
	draw::start();
	return 0;
}

int _stdcall WinMain(void* ci, void* pi, char* cmd, int sw) {
	return main(0, 0);
}