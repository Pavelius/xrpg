#include "draw_color.h"
#include "draw_input.h"
#include "main.h"

using namespace draw;

static locationi* addloc(const char* id, point v) {
	auto p = bsdata<locationi>::add();
	p->id = id;
	p->world.setposition(v);
	return p;
}

static void test_answers() {
	addloc("Covcheg", worldmap::getp(15, 10));
	game.setposition(worldmap::getp(15, 10));
	game.setexplored(worldmap::geti(15, 10), 2);
	draw::setnext(game.playworld);
}

static void test_window() {
	auto push_fore = fore;
	setpositionlu();
	sheader("Test header");
	stext("Some text must be written. This is *bold* version.");
	fore = get(ColorGreen);
	bar(30, 100);
	fore = push_fore;
}

void gamei::main_menu() {
	draw::scene.resurl = "mutant_title";
	draw::scene.window = test_window;
	menui::choose("Main", "city", 0);
}

int main() {
	if(!initialize_translation("ru"))
		return -1;
	draw::initialize("Mutants: Zero point");
	draw::setnext(game.main_menu);
	draw::start();
	return 0;
}

#ifdef _MSC_VER
int __stdcall WinMain(void* ci, void* pi, char* cmd, int sw) {
	return main();
}
#endif