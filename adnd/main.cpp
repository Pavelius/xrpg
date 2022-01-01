#include "bsreq.h"
#include "log.h"
#include "main.h"

int main(int argc, char* argv[]) {
	initialize_translation("ru");
	bsreq::read("rules/basic.txt");
	check_translation();
	if(log::geterrors())
		return -1;
	return 0;
}

int _stdcall WinMain(void* ci, void* pi, char* cmd, int sw) {
	return main(0, 0);
}