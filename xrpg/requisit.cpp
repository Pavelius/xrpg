#include "requisit.h"

BSDATAD(valuei)
BSDATAD(requisiti)

valuei* valuei::find(variant object, variant requisit) {
	valuei f = {object, requisit};
	auto i = bsdata<valuei>::source.find(&f, 0, sizeof(variant)*2);
	if(i==-1)
		return 0;
	return (valuei*)bsdata<valuei>::source.ptr(i);
}

void valuei::add(variant object, variant requisit, int value) {
	auto p = find(object, requisit);
	if(!p) {
		p = bsdata<valuei>::addz();
		p->object = object;
		p->requisit = requisit;
	}
	p->value = value;
}