#include "draw.h"
#include "draw_button.h"
#include "draw_control.h"
#include "draw_figure.h"
#include "main.h"

using namespace draw;

static void field(int x, int& y, int width, controls::control& v) {
	setposition(x, y, width);
	rect rc = {x, y, x + width, y + 300};
	rectf(rc, colors::window);
	rectb(rc, colors::border);
	v.paint(rc);
	y += 300 + metrics::padding;
}

static void test_fields() {
	auto fname = "��������";
	auto name = "�����";
	auto lname = "������������";
	char age = 41;
	char buffer[2048] = {"� ������� ���� ������ �������� ������ �������� ��� ������ ���������, � ���������� �������� ������������ �������� � ������������ � ���������. ��� ��� ���� ������� �����."};
	while(ismodal()) {
		rect rc = {0, 0, getwidth(), getheight()};
		rectf(rc, colors::form);
		int x = 10, y = 10;
		field(x, y, 400, "�������", fname, 100, 0);
		field(x, y, 400, "���", name, 100, 0);
		field(x, y, 400, "��������", lname, 100, 0);
		fieln(x, y, 400, "�������", &age, sizeof(age), 100, 0);
		field(x, y, 400, 3, "��������", buffer, sizeof(buffer), 100, 0);
		buttonl(x, y, "OK", buttonok, KeyEnter);
		buttonl(x, y, getnm("Cancel"), buttoncancel, KeyEscape);
		domodal();
	}
}

static void panel(int x, int& y, controls::list& e) {
	auto w = 200; auto y1 = y;
	checkbox(x, y, w, &e.show_grid_lines, sizeof(e.show_grid_lines), 0, "���������� ����� �����");
	checkbox(x, y, w, &e.hilite_odd_lines, sizeof(e.hilite_odd_lines), 0, "�������� �������� �����");
	checkbox(x, y, w, &e.show_selection, sizeof(e.show_selection), 0, "������������ ����� ��� ������");
	//x += w + metrics::padding; y = y1;
	//checkbox(x, y, w, &e.drop_shadow, sizeof(e.drop_shadow), 0, "����������� ����");
}

static void test_window() {
	struct test_list : public controls::list {
		const char* getname(int line, int column, stringbuilder& sb) const override {
			switch(column) {
			case 0: return bsdata<traiti>::elements[line].id;
			default: return 0;
			}
		}
		int getmaximum() const override {
			return bsdata<traiti>::source.getcount();
		}
	};
	test_list object;
	while(ismodal()) {
		rect rc = {0, 0, getwidth(), getheight()};
		rectf(rc, colors::form);
		int x = 10, y = 10;
		object.paint({x, y, getwidth() - 200, y + 500 - metrics::padding});
		buttonl(x, y, "OK", buttonok, KeyEnter);
		buttonl(x, y, getnm("Cancel"), buttoncancel, KeyEscape);
		domodal();
	}
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

int main() {
	if(!initialize_translation("ru"))
		return -1;
	logmsgv("�������� ������ ������������ � ����� ��� ����������.", 0);
	//draw::setnext(test_table);
	draw::application("Test UI");
	draw::start();
	return 0;
}

#ifdef _MSC_VER
int __stdcall WinMain(void* ci, void* pi, char* cmd, int sw) {
	return main();
}
#endif