#include "archive.h"
#include "code.h"
#include "code_pack.h"
#include "draw.h"
#include "draw_button.h"
#include "draw_control.h"
#include "draw_figure.h"
#include "io_stream.h"
#include "main.h"
#include "string.h"

using namespace draw;

void initialize_picture();

static void field(int x, int& y, int width, controls::control& v) {
	setposition(x, y, width);
	v.view({x, y, x + width, y + 300}, true, true);
	y += 300 + metrics::padding;
}

static void test_fields() {
	auto fname = "Чистяков";
	auto name = "Павел";
	auto lname = "Валентинович";
	char age = 41;
	char list = 1;
	char buffer[2048] = {"В течение игры каждый участник задает действия для своего персонажа, а результаты действий определяются мастером в соответствии с правилами. Все что надо сделать здесь."};
	while(ismodal()) {
		rect rc = {0, 0, getwidth(), getheight()};
		rectf(rc, colors::form);
		int x = 10, y = 10;
		field(x, y, 400, "Фамилия", fname, 100, 0);
		field(x, y, 400, "Имя", name, 100, 0);
		field(x, y, 400, "Отчество", lname, 100, 0);
		fieln(x, y, 400, "Возраст", &age, sizeof(age), 100, 0);
		field(x, y, 400, "Ссылка", &list, sizeof(list), 100, bsdata<traiti>::source, {getenumname}, 0);
		field(x, y, 400, 3, "Описание", buffer, sizeof(buffer), 100, 0);
		buttonl(x, y, "OK", buttonok, KeyEnter);
		buttonl(x, y, getnm("Cancel"), buttoncancel, KeyEscape);
		domodal();
	}
}

static void panel(int x, int& y, controls::list& e) {
	auto w = 200;
	checkbox(x, y, w, &e.show_grid_lines, sizeof(e.show_grid_lines), 0, "Показывать линии сетки");
	checkbox(x, y, w, &e.hilite_odd_lines, sizeof(e.hilite_odd_lines), 0, "Выделять нечетные рядки");
	checkbox(x, y, w, &e.show_selection, sizeof(e.show_selection), 0, "Подсвечивать рядок под мышкой");
	//x += w + metrics::padding; y = y1;
	//checkbox(x, y, w, &e.drop_shadow, sizeof(e.drop_shadow), 0, "Отбрасывает тень");
}

static void test_table() {
	controls::table	object(bsdata<traiti>::source);
	object.addcol("Name", "Text").set(ANREQ(traiti, id));
	object.addcol("Weight", "Number").set(ANREQ(traiti, weight));
	while(ismodal()) {
		rect rc = {0, 0, getwidth(), getheight()};
		rectf(rc, colors::form);
		int x = 10, y = 10;
		field(x, y, getwidth() - 10 * 2 - 200, object);
		auto y1 = 10;
		panel(x + getwidth() - 10 * 2 - 200 + metrics::padding, y1, object);
		buttonl(x, y, "OK", buttonok, KeyEnter);
		buttonl(x, y, getnm("Cancel"), buttoncancel, KeyEscape);
		domodal();
	}
}

static bool test_archive(bool write_mode) {
	io::file file("test.data", write_mode ? StreamWrite : StreamRead);
	archive e(file, write_mode);
	if(!e.signature("TST"))
		return false;
	if(!e.version(0,1))
		return false;
	e.set(bsdata<planeti>::source);
	return true;
}

static bool test_pack() {
	code::pack p1;
	p1.create("test.ui");
	p1.addclass(p1.add("uint"), 0);
	p1.addclass(p1.add("ushort"), 0);
	p1.addclass(p1.add("uchar"), 0);
	p1.addclass(p1.add("int"), 0);
	p1.addclass(p1.add("short"), 0);
	p1.addclass(p1.add("char"), 0);
	for(auto hs : p1.getsymbols()) {
		auto name = p1.getname(hs);
		auto flags = p1.getflags(hs);
	}
	p1.write("test.ast");
	return true;
}

static bool test_string() {
	string t1 = "Answer";
	string t2 = "Duke";
	if(t1 == "Duke1")
		return false;
	if(t1 != "Answer")
		return false;
	return true;
}

#ifdef _DEBUG
void main_util();
#endif

int main() {
#ifdef _DEBUG
	main_util();
#endif
	if(!test_pack())
		return -1;
	if(!test_string())
		return -1;
	if(!test_archive(true))
		return -1;
	if(!initialize_translation("ru"))
		return -1;
	initialize_picture();
	initialize_codeview();
	initialize_codetree();
	initialize_lexers();
	logmsgv("Тостовая строка залогирована в общий лог приложения.", 0);
	draw::initialize("Test UI");
	openurl("battle.bmp");
	openurl("code/RustCode.rs");
	openurl("code/projects/first/main.c2");
	//draw::setnext(test_table);
	//draw::setnext(test_fields);
	draw::setnext(draw::application);
	draw::start();
	return 0;
}

#ifdef _MSC_VER
int __stdcall WinMain(void* ci, void* pi, char* cmd, int sw) {
	return main();
}
#endif
