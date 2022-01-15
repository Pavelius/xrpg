#include "bsreq.h"
#include "draw.h"
#include "log.h"
#include "main.h"

void set_dark_theme();
void initialize_png();

static void test_game() {
	mission.clear();
	mission.add("Marines");
	mission.add("Marines");
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
	draw::setnext(draw::runmission);
#ifdef _DEBUG
	test_game();
#endif // _DEBUG
	draw::start();
	return 0;
}

int _stdcall WinMain(void* ci, void* pi, char* cmd, int sw) {
	return main(0, 0);
}