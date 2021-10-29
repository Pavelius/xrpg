#include "draw.h"
#include "main.h"

void locationi::paint() const {
	auto s = 5;
	auto h = draw::ishilite(s, this);
	draw::field(figure::TrianlgeUp, s);
}