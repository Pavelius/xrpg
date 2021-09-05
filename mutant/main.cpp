#include "draw_color.h"
#include "draw_input.h"
#include "main.h"

using namespace draw;

void initialize_commands();

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

static void avatardr(const char* id) {
	auto push_caret = caret;
	auto push_width = width;
	auto push_fore = fore;
	fore = get(ColorBorder);
	avatar(id);
	fore = get(ColorGreen);
	bar(30, 100);
	fore = get(ColorRed);
	bar(50, 100);
	fore = push_fore;
	caret = push_caret;
	caret.x += width + 4;
	width = push_width;
}

static void avatar_line() {
	setpositionld();
	caret.y -= 22;
	avatardr("stalker");
	avatardr("druid");
	avatardr("northmen");
}

void gamei::main_menu() {
	draw::scene.resurl = "mutant_title";
	menui::choose("Main", "city", 0);
}

int main() {
	if(!initialize_translation("ru"))
		return -1;
	draw::initialize("Mutants: Zero point");
	initialize_commands();
	draw::setnext(game.main_menu);
	draw::start();
	return 0;
}

#ifdef _MSC_VER
int __stdcall WinMain(void* ci, void* pi, char* cmd, int sw) {
	return main();
}
#endif