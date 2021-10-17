#include "draw.h"
#include "draw_simple.h"
#include "main.h"

static fnevent standart_beforemodal;

static void event_ismodal() {
	if(standart_beforemodal)
		standart_beforemodal();
}

void draw::initialize() {
	initialize("Simwar game");
	simpleinitialize();
	image_url = "silentseas";
	standart_beforemodal = pbeforemodal;
	pbeforemodal = event_ismodal;
}