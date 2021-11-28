#include "bsreq.h"
#include "variantlist.h"

void variantlist::match(variant_s type, const char* id, variant v, bool keep) {
	auto pb = begin();
	auto pe = end();
	auto req = bsdata<varianti>::get(type).metadata->find(id);
	if(req && req->type==bsmeta<variant>::meta) {
		auto u = req->offset;
		for(auto p = pb; p < pe; p++) {
			auto pv = (variant*)((char*)p->getpointer() + u);
			if((*pv == v) != keep)
				continue;
			*pb++ = *p;
		}
	}
	count = pb - begin();
}