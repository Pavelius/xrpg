#include "color.h"

#pragma once

namespace draw {
enum color_s : unsigned char {
	NoColor,
	ColorRed, ColorGreen, ColorBlue, ColorYellow, ColorOrange,
	ColorDarkGray, ColorLightBlue, ColorLightGreen,
	ColorBlack, ColorWhite, ColorGray,
	ColorBorder
};
extern color			fore;
color					get(color_s v);
void					set(color_s v);
}
