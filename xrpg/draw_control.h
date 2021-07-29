#include "rect.h"
#include "stringbuilder.h"

#pragma once

typedef bool(*fnvisible)(const void* object);

namespace draw {
struct command {
	const char*				panel;
	const char**			commands;
};
namespace controls {
struct control {
	constexpr control() {}
	virtual ~control() {}
	virtual void			execute(const char* id) {}
	virtual command*		getcommands() const { return 0; }
	virtual const char*		getvalue(const char* id, stringbuilder& sb) const {}
	virtual bool			isallow(const char* id) const {}
	virtual void			paint(const rect& rc) const {}
};
struct list : public control {
	int						origin, current;
	virtual void			cell(const rect& rc, int line, int column) const {}
	int						getorigin() const { return origin; }
	int						getcurrent() const { return current; }
	void					paint(const rect& rc) const override {}
	virtual void			row(const rect& rc, int line) const {}
};
}
}
