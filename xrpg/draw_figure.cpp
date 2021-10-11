#include "draw.h"
#include "draw_figure.h"

void draw::field(figure type, int size) {
	auto push_caret = caret;
	switch(type) {
	case figure::Check:
		caret.x = push_caret.x - size;
		caret.y = push_caret.y - size + 2;
		line(push_caret.x, push_caret.y + size);
		line(push_caret.x + size, push_caret.y - size);
		break;
	case figure::Circle:
		circle(size);
		break;
	case figure::CircleFill:
		circlef(size);
		circle(size);
		break;
	case figure::Rect:
		rectb({push_caret.x - size, push_caret.y - size, push_caret.x + size, push_caret.y + size});
		break;
	case figure::Rect3D:
		rectb3d({push_caret.x - size, push_caret.y - size, push_caret.x + size, push_caret.y + size});
		break;
	case figure::RectFill:
		rectf({push_caret.x - size, push_caret.y - size, push_caret.x + size, push_caret.y + size});
		break;
	case figure::TrianlgeUp:
		caret.x = push_caret.x - size;
		caret.y = push_caret.y - size;
		line(push_caret.x + size, push_caret.y - size);
		line(push_caret.x, push_caret.y + size);
		line(push_caret.x - size, push_caret.y - size);
		break;
	case figure::Trianlge:
		caret.x = push_caret.x - size;
		caret.y = push_caret.y + size;
		line(push_caret.x + size, push_caret.y + size);
		line(push_caret.x, push_caret.y - size);
		line(push_caret.x, push_caret.y + size);
		break;
	case figure::Close:
		caret.x = push_caret.x - size;
		caret.y = push_caret.y - size;
		line(push_caret.x + size, push_caret.y + size);
		caret.x = push_caret.x - size;
		caret.y = push_caret.y + size;
		line(push_caret.x + size, push_caret.y - size);
		break;
	case figure::Cross:
		caret.x = push_caret.x - size;
		caret.x = push_caret.y - size;
		line(push_caret.x + size, push_caret.y + size);
		caret.x = push_caret.x + size;
		caret.x = push_caret.y - size;
		line(push_caret.x - size, push_caret.y + size);
		break;
	default:
		break;
	}
	caret = push_caret;
}

void draw::field(figure type, int size, const char* format) {
	field(type, size);
	if(format) {
		auto push_caret = caret;
		caret.x -= textw(format) / 2;
		caret.y += size + 2;
		text(format);
		caret = push_caret;
	}
}