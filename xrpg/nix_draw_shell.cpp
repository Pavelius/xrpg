#include "crt.h"
#include "draw_button.h"

bool draw::dialog::open(const char* title, char* path, const char* filter, int filter_index, const char* ext) {
	return false;
}

bool draw::dialog::save(const char* title, char* path, const char* filter, int filter_index) {
	return false;
}

bool draw::dialog::choose(struct color& result, struct color* custom) {
	return false;
}

bool draw::dialog::folder(const char* title, char* path) {
	return true;
}