#include "crt.h"
#include "reference.h"

static array strings(1);
BSDATAD(reference)
BSDATAD(valuei)

const char* getstr(unsigned v) {
	return (const char*)strings.ptr(v);
}

unsigned getidx(const char* v) {
	return strings.indexof(strings.addus(v, zlen(v)));
}

static void addref(unsigned index, unsigned parent, const char* id) {
	auto& e = bsdata<reference>::source;
	e.reserve(index + 1);
	auto p = (reference*)e.ptr(index);
	p->id = getidx(id);
	p->parent = parent;
	p->param = 0;
}

void initialize_reference() {
	addref(None, 0, "None");
	addref(Types, 0, "Types");
	addref(Text, Types, "Text");
	addref(Number, Types, "Number");
	addref(ID, Text, "ID");
}
