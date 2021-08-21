#include "crt.h"

#pragma once

class strlib : public array {
	unsigned		add(const char* v, unsigned size);
	unsigned		find(const char* v, unsigned size) const;
public:
	const char*		get(unsigned v) const { return (v == 0xFFFFFFFF) ? "" : (const char*)array::ptr(v); }
	unsigned		get(const char* v);
	unsigned		get(const char* v, unsigned size);
};