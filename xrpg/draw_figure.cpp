#include "draw.h"
#include "draw_figure.h"

void draw::field(int x, int y, figure type, int size) {
	switch(type) {
	case figure::Check:
		line(x - size, y - size + 2, x, y + size);
		line(x + size, y - size, x, y + size);
		break;
	case figure::Circle:
		circle(x, y, size);
		break;
	case figure::CircleFill:
		circlef(x, y, size, fore, 128);
		circle(x, y, size);
		break;
	case figure::Rect:
		rectb({x - size, y - size, x + size, y + size});
		break;
	case figure::Rect3D:
		rectb3d({x - size, y - size, x + size, y + size});
		break;
	case figure::RectFill:
		rectf({x - size, y - size, x + size, y + size});
		break;
	case figure::TrianlgeUp:
		line(x - size, y - size, x + size, y - size);
		line(x + size, y - size, x, y + size);
		line(x - size, y - size, x, y + size);
		break;
	case figure::Trianlge:
		line(x - size, y + size, x + size, y + size);
		line(x + size, y + size, x, y - size);
		line(x, y - size, x, y + size);
		break;
	case figure::Close:
		line(x - size, y - size, x + size, y + size);
		line(x - size, y + size, x + size, y - size);
		break;
	case figure::Cross:
		line(x - size, y, x + size, y);
		line(x, y - size, x, y + size);
		break;
	default:
		break;
	}
}

void draw::field(int x, int y, figure type, int size, const char* format) {
	field(x, y, type, size);
	if(format)
		text(x - textw(format)/2, y + size + 2, format);
}