#include "main.h"

unsigned long gettickcount();

static locationi* addloc(const char* id, const char* name, point v) {
	auto p = bsdata<locationi>::add();
	p->id = id;
	p->name = name;
	p->setposition(v);
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
	an.choosev(0, "Выход", true, "new_character", false);
}

void gamei::main_menu() {
	gui.bitmap = "mutant_title";
	answers an;
	an.add(1, "Начать новую игру");
	an.add(2, "Загрузить сохраненную игру");
	an.add(3, "Перейти на сайт игры");
	auto r = an.choosev(0, "Выход", true, "city", false);
	switch(r) {
	case 1: new_character(); break;
	default:
		break;
	}
}

int main() {
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