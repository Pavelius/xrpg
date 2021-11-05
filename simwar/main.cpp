#include "bsreq.h"
#include "draw.h"
#include "draw_simple.h"
#include "log.h"
#include "main.h"

void initialize_png();

#ifdef _DEBUG
void util_main();
#endif // _DEBUG

int main() {
    srand(randomseed());
    initialize_translation("ru");
    check_translation();
    bsreq::read("rules/test_data.txt");
    eventi::read("rules/events.txt");
    if(log::geterrors())
        return -1;
    initialize_png();
#ifdef _DEBUG
    util_main();
#endif // _DEBUG
    draw::initialize();
    game.initialize();
    game.player = bsdata<playeri>::elements;
    game.player->initialize();
    game.player->refresh();
    game.province = (provincei*)bsdata<provincei>::source.ptr(0);
    //eventi* pe = variant("ShamanCome");
    //game.play(pe);
    draw::setactive(gamei::playermove);
    draw::start();
    return 0;
}

#ifdef _MSC_VER
int __stdcall WinMain(void* ci, void* pi, char* cmd, int sw) {
    return main();
}
#endif