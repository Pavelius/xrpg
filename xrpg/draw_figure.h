#pragma once

enum class figure : unsigned char {
	Check, Circle, CircleFill, Close, Cross,
	Rect, Rect3D, RectFill, Trianlge, TrianlgeUp,
};
namespace draw {
void			field(figure type, int size);
void			field(figure type, int size, const char* format);
}