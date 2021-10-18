#include "draw.h"
#include "draw_figure.h"

void draw::field(figure type, int size) {
	rectpush push;
	switch(type) {
	case figure::Check:
		caret.x = push.caret.x - size;
		caret.y = push.caret.y - size + 2;
		line(push.caret.x, push.caret.y + size);
		line(push.caret.x + size, push.caret.y - size);
		break;
	case figure::Circle:
		circle(size);
		break;
	case figure::CircleFill:
		circlef(size);
		circle(size);
		break;
	case figure::Rect:
		setpos(push.caret.x - size, push.caret.y - size, push.caret.x + size, push.caret.y + size);
		rectb();
		break;
	case figure::Rect3D:
		setpos(push.caret.x - size, push.caret.y - size, push.caret.x + size, push.caret.y + size);
		rectb3d();
		break;
	case figure::RectFill:
		setpos(push.caret.x - size, push.caret.y - size, push.caret.x + size, push.caret.y + size);
		rectf();
		break;
	case figure::TrianlgeUp:
		caret.x = push.caret.x - size;
		caret.y = push.caret.y - size;
		line(push.caret.x + size, push.caret.y - size);
		line(push.caret.x, push.caret.y + size);
		line(push.caret.x - size, push.caret.y - size);
		break;
	case figure::Trianlge:
		caret.x = push.caret.x - size;
		caret.y = push.caret.y + size;
		line(push.caret.x + size, push.caret.y + size);
		line(push.caret.x, push.caret.y - size);
		line(push.caret.x, push.caret.y + size);
		break;
	case figure::Close:
		caret.x = push.caret.x - size;
		caret.y = push.caret.y - size;
		line(push.caret.x + size, push.caret.y + size);
		caret.x = push.caret.x - size;
		caret.y = push.caret.y + size;
		line(push.caret.x + size, push.caret.y - size);
		break;
	case figure::Cross:
		caret.x = push.caret.x - size;
		caret.x = push.caret.y - size;
		line(push.caret.x + size, push.caret.y + size);
		caret.x = push.caret.x + size;
		caret.x = push.caret.y - size;
		line(push.caret.x - size, push.caret.y + size);
		break;
	default:
		break;
	}
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