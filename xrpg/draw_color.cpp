#include "draw.h"
#include "draw_color.h"

color draw::get(color_s v) {
	static color theme_colors[] = {
		{},
		{235, 90, 70}, {97, 189, 79}, {0, 121, 191}, {242, 214, 0}, {255, 159, 26},
		{52, 69, 99}, {0, 194, 224}, {81, 232, 152},
		{0, 0, 0}, {255, 255, 255}, {179, 186, 197},
	};
	switch(v) {
	case ColorBorder: return colors::border;
	default: return theme_colors[v];
	}
}

void draw::set(color_s v) {
	fore = get(v);
}