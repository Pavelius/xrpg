#include "draw.h"
#include "draw_background.h"

using namespace draw;

const char*		draw::background::url;
static point	camera_drag;
static rect		board;

void draw::background::tips() {
	const int step = 32;
	switch(hot.key) {
	case KeyLeft: execute(cbsetsht, camera.x - step, 0, &camera.x); break;
	case KeyRight: execute(cbsetsht, camera.x + step, 0, &camera.x); break;
	case KeyUp: execute(cbsetsht, camera.y - step, 0, &camera.y); break;
	case KeyDown: execute(cbsetsht, camera.y + step, 0, &camera.y); break;
	case MouseWheelUp: execute(cbsetsht, camera.y - step, 0, &camera.y); break;
	case MouseWheelDown: execute(cbsetsht, camera.y + step, 0, &camera.y); break;
	case MouseLeft:
		if(hot.pressed && hot.hilite == board) {
			dragbegin(&camera);
			camera_drag = camera;
		}
		break;
	default:
		if(dragactive(&camera)) {
			hot.cursor = cursor::All;
			if(hot.mouse.x >= 0 && hot.mouse.y >= 0)
				camera = camera_drag + (dragmouse - hot.mouse);
		}
		break;
	}
}

void draw::background::paint() {
	if(!url)
		return;
	auto p = gres(url, "art/background");
	if(!p)
		return;
	auto& fr = p->get(0);
	board.x1 = -camera.x;
	board.y1 = -camera.y;
	if(fr.sx < getwidth()) {
		board.x1 = (getwidth() - fr.sx) / 2;
		camera.x = -board.x1;
	}
	if(fr.sy < getheight()) {
		board.y1 = (getheight() - fr.sy) / 2;
		camera.y = -board.y1;
	}
	board.x2 = board.x1 + fr.sx;
	board.y2 = board.y1 + fr.sy;
	ishilite(board);
	image(board.x1, board.y1, p, 0, 0);
}