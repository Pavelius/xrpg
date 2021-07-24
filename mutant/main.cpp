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
	draw::setbitmap("map");
	addloc("Covcheg", "Ковчег", worldmap::getp(15, 10));
	game.setposition(worldmap::getp(15, 10));
	game.setexplored(worldmap::geti(15, 10), 2);
	//game.setmovement(worldmap::getp(10, 4));
	draw::setnext(game.playworld);
}

int main() {
	draw::initialize();
	test_answers();
	draw::application();
	return 0;
}

#ifdef _MSC_VER
int __stdcall WinMain(void* ci, void* pi, char* cmd, int sw) {
	return main();
}
#endif