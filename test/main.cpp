#include "draw.h"
#include "draw_button.h"
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