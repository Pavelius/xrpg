#include "crt.h"
#include "draw_input.h"
#include "draw_simple.h"
#include "tablecsv.h"

bool readf(const char* url);
void initialize_json();

int main() {
	initialize_json();
	if(!initialize_translation("ru"))
		return -1;
	if(!tablecsvi::read("rules/dnd5/ClericSpellProgression.csv"))
		return -1;
	if(!tablecsvi::read("rules/dnd5/ProficiencyBonusTable.csv"))
		return -1;
	if(!readf("rules/dnd5/core.json"))
		return -1;
	if(!tablecsvi::read("rules/dnd5/Test.csv"))
		return -1;
	draw::initialize("DnD5 simulator");
	draw::setnext(draw::simpleui);
	draw::start();
	return 0;
}

#ifdef _MSC_VER
int __stdcall WinMain(void* ci, void* pi, char* cmd, int sw) {
	return main();
}
#endif