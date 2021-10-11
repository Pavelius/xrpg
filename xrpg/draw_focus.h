#include "rect.h"

#pragma once

namespace draw {
class pushfocus {
	const void*	focus;
	unsigned	bits;
public:
	pushfocus();
	~pushfocus();
};
void			blockfocuskeys();
void			focusbeforemodal();
void			focusinput();
void			focusleavemodal();
bool			isfocused();
bool			isfocused(const void* value, unsigned bits = 0);
bool			isfocused(const rect& rc, const void* value, unsigned bits = 0, bool force_focused = false);
void			setfocus(const void* value, unsigned bits = 0, bool instant = false);
void			setfocusable(const rect& rc, const void* value, unsigned bits = 0);
}