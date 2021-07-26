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

struct test1 {
	template<class T> constexpr static variant_s getkindvalue();
	unsigned char		t;
	unsigned char		v;
	constexpr test1() : t(0), v(0) {}
	template<class T> constexpr test1(T v) : t(getkindvalue<T>()), v(v) {}
	test1(void* v) : t(100), v(0) {}
	test1(const char* v) : t(101), v(0) {}
};
template<> constexpr static variant_s test1::getkindvalue<role_s>() { return Role; }

static test1 st1 = Chronicler;

template<unsigned V> struct test2 {
	constexpr static int value = V;
};

static bool test_overload() {
	test1 m1(Enforcer);
	test1 m2("Test string");
	auto n = test2<test1(Chronicler).v>::value;
	if(n != 1)
		return false;
	return m1.t == 1;
}

int main() {
	test_overload();
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