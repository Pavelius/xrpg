#include "crt.h"
#include "variant.h"

#pragma once

struct varianta : public adat<variant> {
	void			distinct();
	void			select(const array& source);
	void			shuffle();
	variant			choose(const char* title, const char* cancel, bool interactive, const char* resid) const;
	variant         choosedef(const char* title) const;
};