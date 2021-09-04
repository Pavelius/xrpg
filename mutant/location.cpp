#include "main.h"

void locationi::paint() const {
	auto s = 5;
	auto h = draw::ishilite(s, this);
	draw::field(draw::scene.x, draw::scene.y, figure::TrianlgeUp, s);
}