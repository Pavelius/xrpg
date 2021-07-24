#include "main.h"

static worldmap map;

static void show_worldmap() {
	draw::grid();
	map.fow();
}

static void test_answers() {
	draw::setbitmap("map");
	draw::setbackground(show_worldmap);
	map.set(map.geti(15, 10), 2, 1, true);
	map.set(map.geti(15, 10), 1, 2, false);
	answers an;
	an.add(Endure, "Момент");
	an.add(Fight, "Заправка");
	an.choosev("Тестирование данных", "Отмена", true, 0);
}

int main() {
	draw::initialize();
	draw::application();
	test_answers();
	return 0;
}

#ifdef _MSC_VER
int __stdcall WinMain(void* ci, void* pi, char* cmd, int sw) {
	return main();
}
#endif