#include "answers.h"
#include "varianta.h"

void varianta::select(const array& source) {
	auto pe = source.end();
	for(auto p = source.begin(); p < pe; p += source.getsize())
		add((void*)p);
}

variant varianta::choose(const char* title, const char* cancel, bool interactive, const char* resid) const {
	answers an;
	for(auto v : *this)
		an.add(v.getpointer(), v.getname());
	an.sort();
	return (void*)an.choose(title, cancel, interactive, resid, 1);
}

variant varianta::choosedef(const char* title) const {
	if(count==1)
        return data[0];
    return choose(title, 0, true, 0);
}

static int compare_addr(const void* p1, const void* p2) {
	return *((int*)p1) - *((int*)p2);
}

void varianta::distinct() {
	qsort(data, count, sizeof(data[0]), compare_addr);
	auto ps = data;
	auto pe = data + count;
	auto v0 = variant();
	for(auto p = data; p < pe; p++) {
		if(v0 == *p)
			continue;
		*ps++ = *p;
		v0 = *p;
	}
	count = ps - data;
}

void varianta::shuffle() {
	zshuffle(data, count);
}
