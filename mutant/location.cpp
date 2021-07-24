#include "main.h"

void locationi::paint() const {
	auto x = getposition().x;
	auto y = getposition().y;
	auto s = 5;
	auto h = draw::ishilite(x, y, s, this);
	draw::paint(x, y, name, FigureTrianlgeUp, s);
}