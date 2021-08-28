#pragma once

typedef void(*fnevent)();

namespace draw {
struct scenei{
	int				x = 0, y = 0, width = 320, grid = 32;
	const char*		resurl = 0;
	fnevent			background = 0;
	fnevent			window = 0;
};
extern scenei		scene;
void				answerbt(int index, long id, const char* title);
bool				buttonfd(const char* title);
bool				buttonfd(const char* title, unsigned key);
void				fog(int n);
void				grid();
void				simpleui();
void				setposition();
inline void			setposition(int x, int y) { scene.x = x; scene.y = y; }
void				stext(const char* string);
bool				window(bool hilite, const char* string, const char* resid);
}
