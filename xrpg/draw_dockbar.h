#include "rect.h"

#pragma once

struct docki {
	const char*		id;
	int				current, size;
};
namespace draw {
void				dockbar(rect& rc);
}
