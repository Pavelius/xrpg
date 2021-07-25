#include "main.h"

using namespace draw;

static locationi* addloc(const char* id, const char* name, point v) {
	auto p = bsdata<locationi>::add();
	p->id = id;
	p->name = name;
	p->world.setposition(v);
	return p;
}

static void test_answers() {
	addloc("Covcheg", "Ковчег", worldmap::getp(15, 10));
	game.setposition(worldmap::getp(15, 10));
	game.setexplored(worldmap::geti(15, 10), 2);
	draw::setnext(game.playworld);
}

static void new_character() {
	answers an;
	an.add(1, "Начать новую игру");
	an.add(2, "Загрузить сохраненную игру");
	an.add(3, "Перейти на сайт игры");
	an.choose(0, "Выход", true, "new_character");
}

void gamei::main_menu() {
	form.bitmap = "mutant_title";
	menui::choose("main", "city", 0);
}

int main() {
	varianti::localization("debug", true);
	if(!varianti::localization("ru", false))
		return -1;
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