#include "draw.h"
#include "draw_button.h"
#include "draw_control.h"
#include "handler.h"

using namespace draw;

int draw::getimage(const char* id) {
	if(equal(id, "Cut"))
		return 3;
	if(equal(id, "Copy"))
		return 4;
	if(equal(id, "Paste"))
		return 5;
	return 0;
}

unsigned draw::getkey(const char* id) {
	if(equal(id, "Cut"))
		return Ctrl + 'X';
	if(equal(id, "Copy"))
		return Ctrl + 'C';
	if(equal(id, "Paste"))
		return Ctrl + 'V';
	return 0;
}

static void test_fields() {
	auto fname = "Чистяков";
	auto name = "Павел";
	auto lname = "Валентинович";
	char age = 41;
	char buffer[2048] = {"В течение игры каждый участник задает действия для своего персонажа, а результаты действий определяются мастером в соответствии с правилами. Все что надо сделать здесь."};
	while(ismodal()) {
		rect rc = {0, 0, getwidth(), getheight()};
		rectf(rc, colors::form);
		int x = 10, y = 10;
		field(x, y, 400, "Фамилия", fname, 100, 0);
		field(x, y, 400, "Имя", name, 100, 0);
		field(x, y, 400, "Отчество", lname, 100, 0);
		fieln(x, y, 400, "Возраст", &age, sizeof(age), 100, 0);
		field(x, y, 400, 3, "Отчество", buffer, sizeof(buffer), 100, 0);
		buttonl(x, y, "OK", buttonok, KeyEnter);
		buttonl(x, y, getnm("Cancel"), buttoncancel, KeyEscape);
		domodal();
	}
}

static void test_window() {
	static const char* names[] = {"Birds", "Rockets", "Soldiers", "Ore", "Solar"};
	struct test_list : public controls::list {
		const char* getname(int line, int column, stringbuilder& sb) const override {
			switch(column) {
			case 0: return names[line];
			default: return 0;
			}
		}
		int getmaximum() const override {
			return sizeof(names) / sizeof(names[0]);
		}
	};
	test_list object;
	while(ismodal()) {
		rect rc = {0, 0, getwidth(), getheight()};
		rectf(rc, colors::form);
		int x = 10, y = 10;
		object.view({x, y, getwidth() - 10, y + 500 - 10});
		y += 500;
		checkbox(x, y, 400, &object.show_grid_lines, sizeof(object.show_grid_lines), 0, "Показывать линии сетки");
		buttonl(x, y, "OK", buttonok, KeyEnter);
		buttonl(x, y, getnm("Cancel"), buttoncancel, KeyEscape);
		domodal();
	}
}

int main() {
	if(!initialize_translation("ru"))
		return -1;
	draw::initialize("Test UI", 0);
	draw::setnext(test_window);
	draw::application();
	return 0;
}

#ifdef _MSC_VER
int __stdcall WinMain(void* ci, void* pi, char* cmd, int sw) {
	return main();
}
#endif