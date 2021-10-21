#include "variant.h"

#pragma once

struct eventi {
    const char* id;
    variants    tags;
    constexpr explicit operator bool() const { return id != 0; }
    static void read(const char* url);
};
struct eventcasei {
    short       parent; // Event index
    short       id; // Case index, started in every quest from zero
    int         next; // Next case index. If -1 use to promt message
    const char* text; // Text to display
    const char* image; // Possible event image.
    variants    effect; // Some effects
    constexpr explicit operator bool() const { return parent != -1; }
    void        clear();
    bool        isprompt() const { return next == -1; }
};
