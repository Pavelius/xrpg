#include "crt.h"
#include "draw_clipboard.h"

void clipboard::copy(const char* string, long lenght) {
	if(!lenght)
		return;
}

bool clipboard::isallowpaste() {
	return false;
}

const char* clipboard::paste(long* size) {
	return 0;
}
