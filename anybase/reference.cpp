#include "crt.h"
#include "reference.h"

static array		strings(1);
BSDATAD(reference)
BSDATAD(valuei)

static const char* getstr(unsigned v) {
	return (const char*)strings.ptr(v);
}

static unsigned getidx(const char* v) {
	return strings.indexof(strings.addus(v, zlen(v)));
}

static valuei* findval(unsigned parent, unsigned req) {
	for(auto& e : bsdata<valuei>()) {
		if(e.parent == parent && e.requisit == req)
			return &e;
	}
	return 0;
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

unsigned reference::getindex() const {
	return bsdata<reference>::source.indexof(this);
}

unsigned reference::get(unsigned req) const {
	unsigned id;
	const valuei* pv;
	switch(req) {
	case ID: return this->id;
	case Parent: return this->parent;
	case Param: return this->param;
	default:
		id = getindex();
		pv = findval(id, req);
		if(pv)
			return pv->value;
		break;
	}
	return 0;
}

void reference::set(unsigned req, unsigned v) {
	if(req == ID)
		id = v;
	else if(req == Parent)
		parent = v;
	else if(req == Param)
		param = v;
	else {
		auto id = getindex();
		auto pv = findval(id, req);
		if(!pv) {
			pv = bsdata<valuei>::add();
			pv->parent = id;
			pv->requisit = req;
		}
		pv->value = v;
	}
}

void reference::set(unsigned req) {
	if(req == ID)
		id = 0;
	else if(req == Parent)
		parent = 0;
	else if(req == Param)
		param = 0;
	else {
		auto id = getindex();
		auto pv = findval(id, req);
		if(pv)
			pv->clear();
	}
}

void valuei::clear() {
	memset(this, 0, sizeof(*this));
}