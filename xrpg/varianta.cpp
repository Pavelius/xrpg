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
		an.add((size_t)v.getpointer(), v.getname());
	an.sort();
	return (void*)an.choose(title, cancel, interactive, resid);
}