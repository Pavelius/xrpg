#include "crt.h"
#include "draw_input.h"
#include "draw_simple.h"
#include "menu.h"
#include "main.h"
#include "tablecsv.h"

bool readf(const char* url);
void initialize_json();
void initialize_png();
void create_character();

fnevent getcommand(const char* id) {
	if(equal("NewCharacter", id))
		return create_character;
	return 0;
}

static void start_menu() {
	menui::choose("Start", "meet", 0);
}

int main() {
	initialize_json();
	initialize_png();
	if(!initialize_translation("ru"))
		return -1;
	if(!tablecsvi::read("rules/dnd5/ClericSpellProgression.csv"))
		return -1;
	if(!tablecsvi::read("rules/dnd5/ProficiencyBonusTable.csv"))
		return -1;
	if(!readf("rules/dnd5/core.json"))
		return -1;
	draw::initialize("DnD5 simulator");
	draw::scene.resurl = "cave_crystal";
	draw::setnext(start_menu);
	draw::start();
	return 0;
}

#ifdef _MSC_VER
int __stdcall WinMain(void* ci, void* pi, char* cmd, int sw) {
	return main();
}
#endif