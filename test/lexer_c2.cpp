#include "codescan.h"

using namespace codescan;

static lexer::word c2_keywords[] = {
	{"auto"},
	{"import"},
	{"if"},
	{"for"},
	{"while"},
};

BSDATA(lexer) = {
	{"c2", "*.c2", c2_keywords}
};
BSDATAF(lexer)
