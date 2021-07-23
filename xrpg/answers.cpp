#include "answers.h"

int answers::compare(const void* v1, const void* v2) {
	return strcmp(((answers::element*)v1)->text, ((answers::element*)v2)->text);
}

void answers::addv(long id, const char* text, const char* format) {
	auto p = elements.add();
	p->id = id;
	p->text = sc.get();
	sc.addv(text, format);
	sc.addsz();
}

void answers::sort() {
	qsort(elements.data, elements.count, sizeof(elements.data[0]), compare);
}

long answers::random() const {
	if(!elements.count)
		return 0;
	return elements.data[rand() % elements.count].id;
}

const char* answers::getname(long v) {
	for(auto& e : elements) {
		if(e.id == v)
			return e.text;
	}
	return 0;
}