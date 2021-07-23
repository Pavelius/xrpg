#include "main.h"

static void worldmap() {
	draw::grid();
	draw::fog(0, 0, 0xFF);
	draw::fog(1, 1, 128);
	draw::fog(1, 0, 128);
	draw::fog(0, 1, 128);
}

static void test_answers() {
	draw::setbitmap("map");
	draw::setbackground(worldmap);
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