#include "main.h"

void locationi::paint() const {
	auto x = world.getposition().x;
	auto y = world.getposition().y;
	auto s = 5;
	auto h = draw::ishilite(x, y, s, this);
	draw::paint(x, y, getnm(id), FigureTrianlgeUp, s);
}