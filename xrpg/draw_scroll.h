#include "rect.h"

#pragma once

namespace metrics {
extern int				scroll;
}

namespace draw {
class scroll {
	int*				origin;
	int					page, maximum, wheel;
	rect				work, client;
	bool				horizontal;
	static void			callback();
public:
	typedef void (scroll::*proc)(int param);
	constexpr scroll() : origin(0), page(), maximum(), wheel(1), work(), client(), horizontal(false) {}
	scroll(int& origin, int page, int maximum, const rect& client, bool horizontal = false, int wheel = 1);
	void				correct();
	void				execute(proc p, int param) const;
	rect				getslide() const;
	void				input();
	bool				ishilite() const;
	bool				isvisible() const { return origin && work; }
	void				setorigin(int v) { if(origin) { *origin = v; correct(); } }
	void				view(bool focused);
};
}