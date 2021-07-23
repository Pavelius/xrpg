#include "main.h"

typedef datamap<25 * 20> fowa;

static fowa fow;

static void paint_fow() {
	for(auto x = 0; x < 25; x++) {
		for(auto y = 0; y < 25; y++) {
			auto v = fow.get(y * 25 + x);
			switch(v) {
			case 0: draw::fog(x, y, 0xFF); break;
			case 1: draw::fog(x, y, 128); break;
			default: break;
			}
		}
	}
}

static void worldmap() {
	draw::grid();
	paint_fow();
}

static void test_answers() {
	draw::setbitmap("map");
	draw::setbackground(worldmap);
	fow.set(2 * 25 + 2, 2);
	fow.set(2 * 25 + 1, 1);
	fow.set(2 * 25 + 3, 1);
	fow.set(1 * 25 + 2, 1);
	fow.set(1 * 25 + 1, 1);
	fow.set(1 * 25 + 3, 1);
	fow.set(3 * 25 + 1, 1);
	fow.set(3 * 25 + 2, 1);
	fow.set(3 * 25 + 3, 1);
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