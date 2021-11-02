#include "command.h"

#pragma once

namespace draw {
void				windows(const command* source);
namespace commands {
extern command*	    source;
void				paint();
}
}
