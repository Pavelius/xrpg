#include "main.h"
#include "log.h"

bool readf(const char* url);
void initialize_json();
void initialize_png();

fnevent getcommand(const char* id) {
	return 0;
}

void util_main();

static bool test_variant() {
    variant v = "City1";
    return v.operator bool();
}

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
	if(!readf("rules/monsters.json"))
		return -1;
	if(!readf("rules/scenarios.json"))
		return -1;
	if(!eventi::read("rules/city.txt", city_events))
		return -1;
    if(log::geterrors())
        return -1;
	//util_main();
	draw::initializex();
	return 0;
}

#ifdef _MSC_VER
int __stdcall WinMain(void* ci, void* pi, char* cmd, int sw) {
	return main();
}
#endif