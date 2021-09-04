#pragma once

enum class figure : unsigned char {
	Check, Circle, CircleFill, Close, Cross,
	Rect, Rect3D, RectFill, Trianlge, TrianlgeUp,
};
namespace draw {
void			field(int x, int y, figure type, int size);
void			field(int x, int y, figure type, int size, const char* format);
}