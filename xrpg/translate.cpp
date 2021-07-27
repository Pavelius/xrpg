#include "crt.h"

struct translate {
	const char*			id;
	const char*			name;
	static void			initialize(const char* lid);
};
template<> array bsdata<translate>::source(sizeof(translate));
static array source_text(sizeof(translate));

bool readl(const char* url, array& source, unsigned* fields, int fields_count, unsigned* special, int special_count);

static int compare(const void* v1, const void* v2) {
	auto p1 = (const translatei*)v1;
	auto p2 = (const translatei*)v2;
	return strcmp(p1->id, p2->id);
}

static void update_elements(array& ei) {
	if(ei.getcount()==0)
		return;
	qsort(ei.data, ei.getcount(), ei.getsize(), compare);
}

void translate::initialize(const char* lid) {
	auto p = bsdata<translatei>::add();
	p->id = "copy";
	p->name = "Copy";
	p = bsdata<translatei>::add();
	p->id = "cut";
	p->name = "Cut";
	update_elements(bsdata<translatei>::source);
	update_elements(source_text);
}

const char* szt(const char* id) {
	if(!id || id[0]==0)
		return "";
	translatei key = {id, 0};
	auto p = (translatei*)bsearch(&key, bsdata<translatei>::source.data, bsdata<translatei>::source.getcount(), bsdata<translatei>::source.getsize(), compare);
	if(!p || !p->name)
		return id;
	return p->name;
}