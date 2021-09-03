#include "draw_control.h"

#pragma once

namespace draw {
namespace controls {
class valuelist : public list {
	struct content {
		const char*		name;
		size_t			value;
		char			image;
	};
	vector<content>		source;
public:
	bool				always_focus = false;
	void				add(const char* name, size_t value = 0, int image = -1);
	void				clear() { source.clear(); }
	const char*			getcurrentname() const;
	size_t				getcurrentvalue() const;
	int					getmaximum() const override { return source.getcount(); }
	const char*			getname(int line, int column, stringbuilder& sb) const override { return source[line].name; }
	int					getrowimage(int line) const override { return source[line].image; }
	bool				isfocused() const override;
};
}
}