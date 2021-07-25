#include "crt.h"
#include "variant.h"

#pragma once

class varianta : public adat<variant> {
public:
	void			select(const array& source);
	variant			choose(const char* title, const char* cancel, bool interactive, const char* resid) const;
};