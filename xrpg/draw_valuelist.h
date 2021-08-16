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
	void				add(const char* name);
	void				clear() { source.clear(); }
	const char*			getcurrentname() const;
	int					getmaximum() const override { return source.getcount(); }
	const char*			getname(int line, int column, stringbuilder& sb) const override;
	bool				isfocused() const override;
};
}
}