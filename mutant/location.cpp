#include "main.h"

void locationi::paint() const {
	auto s = 5;
	auto h = draw::ishilite(s, this);
	draw::field(draw::caret.x, draw::caret.y, figure::TrianlgeUp, s);
}