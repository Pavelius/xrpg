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

void gamei::main_menu() {
	form.bitmap = "mutant_title";
	menui::choose("Main", "city", 0);
}

static bool test_overload() {
	variant m1("Enforcer");
	variant m2(Enforcer);
	return m2.type == Role;
}

int main() {
	if(!initialize_translation("ru"))
		return -1;
	test_overload();
	draw::initialize();
	draw::setnext(game.main_menu);
	draw::application();
	return 0;
}

#ifdef _MSC_VER
int __stdcall WinMain(void* ci, void* pi, char* cmd, int sw) {
	return main();
}
#endif