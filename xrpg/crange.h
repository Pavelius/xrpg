#include "crt.h"

#pragma once

struct crange {
	int         min, max;
	explicit constexpr operator bool() const { return max != 0; }
	constexpr int percent(int v) const { return (max == min) ? 0 : (v - min) * 100 / (max - min); }
	int         random() const { return xrand(min, max); }
};
