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
	adat<element, 32>	elements;
public:
	answers() : sc(buffer) {}
	constexpr operator bool() const { return elements.count != 0; }
	static fnevent		beforepaint;
	static fnevent		afterpaint;
	void				add(long id, const char* name, ...) { addv(id, name, xva_start(name)); }
	void				addv(long id, const char* name, const char* format);
	const element*		begin() const { return elements.data; }
	long				choose(const char* title, const char* cancel_text, bool interactive, const char* resid, int column_count = -1, const char* header = 0) const;
	void				clear();
	static int			compare(const void* v1, const void* v2);
	const element*		end() const { return elements.end(); }
	int					getcount() const { return elements.getcount(); }
	const char*			getname(long v);
	long				random() const;
	void				sort();
};
