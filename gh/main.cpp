#include "main.h"
#include "draw_input.h"
#include "draw_simple.h"
#include "tablecsv.h"

bool readf(const char* url);
void initialize_json();
void initialize_png();

fnevent getcommand(const char* id) {
	return 0;
}

void start_menu() {
	menui::choose("Start", 0, 0);
}

static bool test_players() {
	auto p = (playeri*)bsdata<playeri>::source.ptr(0);
	return p->health[0] != 0;
}

int main() {
	initialize_json();
	initialize_png();
	if(!initialize_translation("ru"))
		return -1;
	if(!readf("rules/core.json"))
		return -1;
	if(!readf("rules/players.json"))
		return -1;
	if(!test_players())
		return - 1;
	draw::initialize("GH simulator");
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