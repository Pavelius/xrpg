#include "main.h"

static worldmap map;
unsigned long gettickcount();

static void update_moveables() {
	auto tick = gettickcount();
	for(auto& e : bsdata<locationi>())
		e.moving(e.getvelocity(), tick, 1000);
}

static void show_worldmap() {
	draw::grid();
	for(auto& e : bsdata<locationi>()) {
		if(e)
			e.paint();
	}
	map.fow();
}

void scenedefault() {
	draw::scene(show_worldmap, update_moveables);
}

static locationi* addloc(const char* id, const char* name, point v) {
	auto p = bsdata<locationi>::add();
	p->id = id;
	p->name = name;
	p->setposition(v);
	return p;
}

static void test_answers() {
	draw::setbitmap("map");
	map.set(map.geti(15, 10), 2, 1, true);
	map.set(map.geti(15, 10), 1, 2, false);
	addloc("Covcheg", "Ковчег", worldmap::getp(15, 10));
	draw::setnext(scenedefault);
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