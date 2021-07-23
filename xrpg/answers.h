#include "crt.h"
#include "stringbuilder.h"

#pragma once

class answers {
	char				buffer[2048];
	stringbuilder		sc;
	struct element {
		long			id;
		const char*		text;
	};
public:
	answers() : sc(buffer) {}
	adat<element, 32>	elements;
	void				add(long id, const char* name, ...) { addv(id, name, xva_start(name)); }
	void				addv(long id, const char* name, const char* format);
	long				choosev(const char* title, const char* cancel_text, bool interactive, const char* resid, bool portraits = false) const;
	static int			compare(const void* v1, const void* v2);
	const char*			getname(long v);
	long				random() const;
	void				sort();
};
