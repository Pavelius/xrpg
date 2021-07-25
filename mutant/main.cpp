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
	addloc("Covcheg", "������", worldmap::getp(15, 10));
	game.setposition(worldmap::getp(15, 10));
	game.setexplored(worldmap::geti(15, 10), 2);
	draw::setnext(game.playworld);
}

static void new_character() {
	answers an;
	an.add(1, "������ ����� ����");
	an.add(2, "��������� ����������� ����");
	an.add(3, "������� �� ���� ����");
	an.choose(0, "�����", true, "new_character");
}

static void test_window() {
	auto player = game.getplayer();
	if(!player)
		return;
	auto gender = player->getgender();
	if(!gender)
		return;
	int x, y, width;
	dialogul(x, y, width);
	header(x, y, width, "%1 - %-3 %-2",
		player->getname(),
		bsdata<rolei>::elements[player->getrole()].name,
		bsdata<genderi>::elements[gender].name);
	for(auto& e : bsdata<attributei>()) {
		text(x, y, e.name);
		y += texth();
	}
}

void gamei::main_menu() {
	form.bitmap = "mutant_title";
	form.window = test_window;
	menui::choose("main", "city", 0);
}

int main() {
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