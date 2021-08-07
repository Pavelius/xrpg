#include "crt.h"
#include "command.h"
#include "draw.h"
#include "draw_button.h"

BSDATA(command) = {
	{"Add", 0, F3, 9},
	{"Change", 0, F2, 10},
	{"Choose", 0, KeyEnter},
	{"Copy", 0, Ctrl + 'C', 3},
	{"Create", 0, 0, 0},
	{"Cut", 0, Ctrl + 'X', 4},
	{"MoveDown", 0, 0, 22},
	{"MoveUp", 0, 0, 21},
	{"Open", 0, 0, 1},
	{"Paste", 0, Ctrl + 'V', 5},
	{"Remove", 0, KeyDelete, 19},
	{"Save", 0, Ctrl + 'S', 2},
	{"Settings", 0, 0, 16},
	{"SortAs", 0, 0, 11},
	{"SortDs", 0, 0, 12},
};
BSDATAF(command)

int draw::getimage(const char* id) {
	for(auto& e : bsdata<command>()) {
		if(strcmp(e.id, id) == 0)
			return e.image;
	}
	return -1;
}

unsigned draw::getkey(const char* id) {
	for(auto& e : bsdata<command>()) {
		if(strcmp(e.id, id) == 0)
			return e.key;
	}
	return 0;
}