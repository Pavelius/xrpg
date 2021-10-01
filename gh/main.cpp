#include "main.h"

bool readf(const char* url);
void initialize_json();
void initialize_png();

fnevent getcommand(const char* id) {
	return 0;
}

void util_main();

int main() {
    srand(randomseed());
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
	//util_main();
	draw::initializex();
	return 0;
}

#ifdef _MSC_VER
int __stdcall WinMain(void* ci, void* pi, char* cmd, int sw) {
	return main();
}
#endif