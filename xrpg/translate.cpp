#include "crt.h"

namespace {
struct translatei {
	const char*			id;
	const char*			name;
};
}
template<> array bsdata<translatei>::source(sizeof(translatei));

static int compare(const void* v1, const void* v2) {
	auto p1 = (const translatei*)v1;
	auto p2 = (const translatei*)v2;
	return strcmp(p1->id, p2->id);
}

static void update_elements() {
	auto& ei = bsdata<translatei>::source;
	if(ei.getcount()==0)
		return;
	qsort(ei.data, ei.getcount(), ei.getsize(), compare);
}

void inittranslate() {
	auto p = bsdata<translatei>::add();
	p->id = "copy";
	p->name = "Копировать";
	p = bsdata<translatei>::add();
	p->id = "cut";
	p->name = "Вырезать";
	update_elements();
}

const char* szt(const char* id) {
	translatei key = {id, 0};
	auto p = (translatei*)bsearch(&key, bsdata<translatei>::source.data, bsdata<translatei>::source.getcount(), bsdata<translatei>::source.getsize(), compare);
	if(!p || !p->name)
		return "";
	return p->name;
}