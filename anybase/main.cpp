#include "main.h"
#include "draw_button.h"
#include "draw_input.h"

void initialize_json();
void initialize_png();
void initialize_reference();

int main() {
	initialize_json();
	initialize_png();
	if(!initialize_translation("ru"))
		return -1;
	initialize_reference();
	draw::initialize("No SQL database Example");
	draw::setnext(draw::application);
	draw::start();
	return 0;
}

#ifdef _MSC_VER
int __stdcall WinMain(void* ci, void* pi, char* cmd, int sw) {
	return main();
}
#endif