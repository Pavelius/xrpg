#include "draw_control.h"

#pragma once

namespace draw {
namespace controls {
class valuelist : public list {
	struct content {
		const char*		name;
	};
	vector<content>		source;
public:
	bool				always_focus = false;
	bool				auto_select = false;
	void				add(const char* name);
	void				appear(int x, int y, int width = 300);
	void				clear() { source.clear(); }
	int					getmaximum() const override { return source.getcount(); }
	const char*			getname(int line, int column, stringbuilder& sb) const override;
	bool				isfocused() const override;
};
}
}