#include "main.h"

BSDATA(menui) = {
	{"Main", "NewGame", character::create_new},
	{"Main", "LoadGame"},
	{"Main", "Settings", 0, "machine", {MenuBack}},
	{"Main", "About"},
	{"Main", "Exit"},
	{"Settings", "Video"},
	{"Settings", "Sound"},
	{"Settings", "Rules"},
};
BSDATAF(menui)