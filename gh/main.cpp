#include "bsreq.h"
#include "main.h"
#include "draw.h"
#include "draw_input.h"
#include "draw_simple.h"

bool readf(const char* url);
void initialize_json();
void initialize_png();

fnevent getcommand(const char* id) {
	return 0;
}

static variant choose_cards(variant player, int level) {
	varianta collection;
	for(auto& e : bsdata<cardi>()) {
		if(e.owner != player)
			continue;
		if(e.level != level)
			continue;
		collection.add(&e);
	}
	return collection.choose(player.getname(), "Cancel", true, 0);
}

void start_menu() {
	choose_cards("Brute", 0);
	menui::choose("Start", 0, 0);
}

static bool test_players() {
	auto p = (playeri*)bsdata<playeri>::source.ptr(0);
	p->buildcombatdeck();
	game.buildcombatdeck();
	return p->health[0] != 0;
}

void util_main();

int main() {
	initialize_json();
	initialize_png();
	if(!initialize_translation("ru"))
		return -1;
	if(!readf("rules/core.json"))
		return -1;
	if(!readf("rules/players.json"))
		return -1;
	//if(!readf("rules/monsters.json"))
	//	return -1;
	if(!test_players())
		return - 1;
	//util_main();
	draw::initialize("GH simulator");
    draw::pausetime = 1000;
	draw::scene.resurl = "gloomhaven";
	draw::setnext(start_menu);
	draw::start();
	return 0;
}

#ifdef _MSC_VER
int __stdcall WinMain(void* ci, void* pi, char* cmd, int sw) {
	return main();
}
#endif