#include "draw_control.h"
#include "draw_focus.h"

using namespace draw;

static void standart_domodal() {
	hot.key = draw::rawinput();
	if(!hot.key)
		exit(0);
	if(inputfocus())
		return;
}

void ismodal_update() {
	domodal = standart_domodal;
}

void ismodal_leaving() {
}

void field_save_and_clear();

void draw::updatefocus() {
	field_save_and_clear();
}

void draw::tooltips(const char* format, ...) {
}

static void test_fields() {
	auto fname = "Чистяков";
	auto name = "Павел";
	auto lname = "Валентинович";
	char age = 41;
	while(ismodal()) {
		rect rc = {0, 0, getwidth(), getheight()};
		rectf(rc, colors::form);
		int x = 10, y = 10;
		field(x, y, 400, "Фамилия", fname, 100, 0);
		field(x, y, 400, "Имя", name, 100, 0);
		field(x, y, 400, "Отчество", lname, 100, 0);
		fieldi(x, y, 400, "Возраст", &age, sizeof(age), 100, 0);
		buttonl(x, y, "OK", buttonok, KeyEnter);
		buttonl(x, y, getnm("Cancel"), buttoncancel, KeyEscape);
		domodal();
	}
}

int main() {
	if(!initialize_translation("ru"))
		return -1;
	draw::initialize("Test UI", 0);
	draw::setnext(test_fields);
	draw::application();
	return 0;
}

#ifdef _MSC_VER
int __stdcall WinMain(void* ci, void* pi, char* cmd, int sw) {
	return main();
}
#endif