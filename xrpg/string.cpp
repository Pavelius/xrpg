#include "crt.h"
#include "string.h"
#include "stringbuilder.h"

const char* string::get() const {
	char temp[512]; stringbuilder sb(temp);
	sb.addnz(p, size);
	return szdup(temp);
}

bool string::equal(const string& v) const {
	return size == v.size && memcmp(p, v.p, size) == 0;
}