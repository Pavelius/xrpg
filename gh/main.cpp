#include "bsreq.h"
#include "draw.h"
#include "main.h"
#include "log.h"

bool readf(const char* url);
void initialize_json();
void initialize_png();

fnevent getcommand(const char* id) {
	return 0;
}

void util_main();

static bool test_bsreq() {
    int result;
    for(auto& e : bsdata<summoni>()) {
        result++;
    }
    return result!=2;
}

int main() {
    srand(randomseed());
	initialize_json();
	initialize_png();
	if(!initialize_translation("ru"))
		return -1;
    bsreq::read("rules/test_cards.txt");
    if(!test_bsreq())
        return -1;
    return 0;
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
	draw::simpleinitialize();
	draw::initializex();
	return 0;
}

#ifdef _MSC_VER
int __stdcall WinMain(void* ci, void* pi, char* cmd, int sw) {
	return main();
}
#endif