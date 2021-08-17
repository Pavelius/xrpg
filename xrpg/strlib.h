#include "crt.h"

#pragma once

struct strlib {
	array			source;
	unsigned		add(const char* v, unsigned size);
	unsigned		find(const char* v, unsigned size) const;
	const char*		get(unsigned v) const { return (v == 0xFFFFFFFF) ? "" : (const char*)source.ptr(v); }
	unsigned		get(const char* v);
	unsigned		get(const char* v, unsigned size);
};