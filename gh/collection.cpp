#include "main.h"

static bool matchv(variant v, variant e) {
    switch(v.type) {
    case Object: return bsdata<object>::get(v.value).match(e);
    default: return false;
    }
    return false;
}

void collection::match(variant e, bool keep) {
    auto p = begin();
    auto pe = end();
    for(auto v : *this) {
        auto r = matchv(v, e);
        if(r!=keep)
            continue;
        *p++ = v;
    }
    count = p - begin();
}