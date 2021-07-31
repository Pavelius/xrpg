#include "rect.h"

#pragma once

namespace draw {
bool			isfocused();
bool			isfocused(const void* value, unsigned bits = 0);
bool			isfocused(const rect& rc, const void* value, unsigned bits = 0);
void			setfocus(const void* value, unsigned bits = 0, bool instant = false);
void			setfocusable(const rect& rc, const void* value, unsigned bits = 0);
}