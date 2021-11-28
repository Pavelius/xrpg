#include "varianta.h"

#pragma once

struct variantlist : public varianta {
	void	match(variant_s type, const char* id, variant v, bool keep);
};