#include "win.h"

void sleep(unsigned v) {
	Sleep(v);
}

unsigned long gettickcount() {
	return GetTickCount();
}