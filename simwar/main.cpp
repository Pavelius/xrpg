#include "bsreq.h"
#include "log.h"
#include "main.h"

static bool test_data() {
    auto pr = bsdata<resourcei>::source_ptr;
    auto pu = bsdata<uniti>::source_ptr;
    auto& e1 = *((uniti*)pu->ptr(0)); // Speadmens
    if(e1.stats.get(Defend)!=2)
        return false;
    auto& e2 = bsdata<uniti>::get(1);
    auto& e3 = bsdata<uniti>::get(2);
    return true;
}

int main() {
    bsreq::read("rules/test_data.txt");
    if(log::geterrors())
        return -1;
    if(!test_data())
        return -2;
    return 0;
}