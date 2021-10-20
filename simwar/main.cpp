#include "bsreq.h"
#include "draw.h"
#include "draw_simple.h"
#include "log.h"
#include "main.h"

static bool test_data() {
    auto pr = bsdata<resourcei>::source_ptr;
    if(!pr || !pr->getcount())
        return false;
    auto pr1 = (resourcei*)pr->ptr(1);
    auto pu = bsdata<uniti>::source_ptr;
    if(!pu)
        return pu;
    auto& u1 = bsdata<uniti>::get(1);
    auto at = u1.get(Attack);
    auto rd = u1.get(Raid);
    auto& e2 = bsdata<provincei>::get(1);
    auto& e3 = bsdata<provincei>::get(2);
    return true;
}

void initialize_png();

int main() {
    if(!initialize_translation("ru"))
        return -1;
    bsreq::read("rules/test_data.txt");
    eventi::read("rules/events.txt");
    if(log::geterrors())
        return -1;
    if(!test_data())
        return -2;
    initialize_png();
    draw::initialize();
    game.initialize();
    game.player = bsdata<playeri>::elements;
    game.player->initialize();
    game.province = (provincei*)bsdata<provincei>::source.ptr(0);
    eventi* pe = variant("ShamanCome");
    game.play(pe);
    draw::setnext(draw::maketurn);
    draw::start();
    return 0;
}

#ifdef _MSC_VER
int __stdcall WinMain(void* ci, void* pi, char* cmd, int sw) {
    return main();
}
#endif