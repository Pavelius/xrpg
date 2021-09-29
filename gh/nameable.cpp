#include "stringact.h"
#include "main.h"

void nameable::act(const char* format, ...) const {
    char temp[4096];
    stringact sb(temp, getname(), getgender());
    sb.addv(format, xva_start(format));
    draw::status(temp);
}

gender_s nameable::getgender() const {
    switch(kind.type) {
    case Player: return bsdata<playeri>::get(kind.value).gender;
    default: return Male;
    }
}
