#include "crt.h"
#include "stringbuilder.h"

#pragma once

class answers {
	char				buffer[2048];
	stringbuilder		sc;
	struct element {
		const void*		value;
		const char*		text;
	};
	adat<element, 32>	elements;
public:
	static fnevent		afterpaint;
	static fnevent		beforepaint;
	static bool			show_tips;
	answers() : sc(buffer) {}
	constexpr operator bool() const { return elements.count != 0; }
	void				add(const void* value, const char* name, ...) { addv(value, name, xva_start(name)); }
	void				addv(const void* value, const char* name, const char* format);
	const element*		begin() const { return elements.data; }
	void*				choose(const char* title, const char* cancel_text, bool interactive, const char* resid, int column_count = -1, const char* header = 0) const;
	void				clear();
	static int			compare(const void* v1, const void* v2);
	const element*		end() const { return elements.end(); }
	int					getcount() const { return elements.getcount(); }
	const char*			getname(void* v);
	void				modal(const char* title, const char* cancel) const;
	void*				random() const;
	void				sort();
};
