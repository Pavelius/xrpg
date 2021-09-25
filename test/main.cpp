#include "answers.h"
#include "archive.h"
#include "dialog.h"
#include "draw.h"
#include "draw_button.h"
#include "draw_control.h"
#include "draw_input.h"
#include "draw_figure.h"
#include "draw_simple.h"
#include "io_stream.h"
#include "lexer.h"
#include "main.h"
#include "package.h"
#include "string.h"
#include "variant.h"

using namespace draw;

BSDATA(varianti) = {
	{""},
	{"Trait", 0, bsdata<traiti>::source_ptr},
	{"Planet", 0, bsdata<planeti>::source_ptr},
};
BSDATAF(varianti)
BSDATAC(variant, 1024)

void initialize_picture();

static void test_table() {
	controls::table	object(bsdata<traiti>::source);
	object.addcol("Name", "Text").set(ANREQ(traiti, id));
	object.addcol("Weight", "Number").set(ANREQ(traiti, weight));
	while(ismodal()) {
		rect rc = {0, 0, getwidth(), getheight()};
		rectf(rc, colors::form);
		int x = 10, y = 10;
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
	if(!e.version(0, 1))
		return false;
	e.set(bsdata<planeti>::source);
	return true;
}

static bool test_pack() {
	code::package p1;
	p1.create("test.ui");
	p1.addclass(p1.add("uint"), 0);
	p1.addclass(p1.add("ushort"), 0);
	p1.addclass(p1.add("uchar"), 0);
	p1.addclass(p1.add("int"), 0);
	p1.addclass(p1.add("short"), 0);
	p1.addclass(p1.add("char"), 0);
	for(auto hs : p1.getsymbols()) {
		p1.getname(hs);
		p1.getflags(hs);
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

static void test_scene() {
}

#ifdef _DEBUG
void main_util();
#endif

static void common_status() {
	static command commands[] = {
		{"Test", test_scene},
		{}};
	auto push_caret = caret;
	auto push_width = width;
	setpositionld();
	windows(commands);
	width = push_width;
	caret = push_caret;
}

static void test_appliaction() {
	initialize_picture();
	initialize_codeview();
	initialize_codetree();
	initialize_lexers();
	draw::initialize("Test UI");
	openurl("battle.bmp");
	//openurl("code/RustCode.rs");
	openurl("code/projects/first/main.c2");
	bsdata<code::lexer>::elements[0].setgrammar();
	if(!code::parse("10*2 + 5", "expression"))
		return;
	draw::setnext(draw::application);
}

static void test_simpleui() {
	draw::initialize("Test UI");
	scene.resurl = "wild";
	auto push_proc = scene.window;
	scene.window = common_status;
	answers an;
	an.add(1, "Test #%1i", 1);
	an.add(2, "Test #%1i", 2);
	an.choose("��������� ���������� �����", getnm("Cancel"), true, "space");
	scene.window = push_proc;
}

int main() {
#ifdef _DEBUG
	main_util();
#endif
	//if(!test_pack())
	//	return -1;
	//if(!test_string())
	//	return -1;
	if(!initialize_speech("ru"))
		return -1;
	if(!initialize_translation("ru"))
		return -1;
	//draw::setnext(test_table);
	//draw::setnext(test_fields);
	//test_answers();
	//test_simpleui();
	test_appliaction();
	draw::start();
	return 0;
}

#ifdef _MSC_VER
int __stdcall WinMain(void* ci, void* pi, char* cmd, int sw) {
	return main();
}
#endif
