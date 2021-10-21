#include "crt.h"
#include "calendar.h"

BSDATA(calendari) = {
	{"March", Spring, 0},
	{"April", Spring, 1},
	{"May", Spring, 2},
	{"June", Summer, 0},
	{"July", Summer, 1},
	{"August", Summer, 2},
	{"September", Autum, 0},
	{"October", Autum, 1},
	{"November", Autum, 2},
	{"December", Autum, 0},
	{"January", Winter, 1},
	{"February", Winter, 2},
};
BSDATAF(calendari)

BSDATA(monthparti) = {
	{"StartMonth"},
	{"MiddleMonth"},
	{"EndMonth"},
};
BSDATAF(monthparti)

BSDATA(seasoni) = {
	{"Spring"},
	{"Summer"},
	{"Autumn"},
	{"Winter"},
};
BSDATAF(seasoni)