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

const char* get_tab_text(const void* object, stringbuilder& sb) {
	return (const char*)object;
}

static void test_window_proc() {
	static int current_tab;
	int x, y, width;
	dialogul(x, y, width);
	char temp[260]; stringbuilder sb(temp);
	sb.add("You have %ScrapAxe.");
	text(x, y, temp);
	y += texth() + gui.padding;
	static const char* tabs[] = {"Вывод", "Ввод", "Параметры"};
	int hilite;
	auto result = draw::tabv({x, y, x + width, y + 260}, false, false, (void**)&tabs, 0, 3, current_tab, &hilite, get_tab_text);
	if(result == 1)
		execute(setint, hilite, 0, &current_tab);
	int x2 = x + width;
	int y2 = y + 263;
	buttonr(x2, y2, "Назад", buttoncancel);
	buttonr(x2, y2, "OK", buttonok);
}

static void test_window() {
	form.window = test_window_proc;
	form.bitmap = "mutant_title";
	menui::choose("Main", "city", 0);
}

int main() {
	if(!initialize_translation("ru"))
		return -1;
	test_overload();
	draw::initialize();
	//draw::setnext(game.main_menu);
	draw::setnext(test_window);
	draw::application();
	return 0;
}

#ifdef _MSC_VER
int __stdcall WinMain(void* ci, void* pi, char* cmd, int sw) {
	return main();
}
#endif