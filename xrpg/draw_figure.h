#pragma once

enum class figure : unsigned char {
	Check, Circle, CircleFill, Close, Cross,
	Rect, RectFill, Trianlge, TrianlgeUp,
};
namespace draw {
void			field(int x, int y, figure type, int size);
}