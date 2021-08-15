#include "crt.h"
#include "string.h"
#include "stringbuilder.h"

const char* string::get() const {
	char temp[512]; stringbuilder sb(temp);
	sb.addnz(p, count);
	return szdup(temp);
}