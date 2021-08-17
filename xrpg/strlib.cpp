#include "strlib.h"

unsigned strlib::add(const char* v, unsigned size) {
	auto n = source.getcount();
	source.reserve(n + size + 1);
	auto p = (char*)source.ptr(n);
	memcpy(p, v, size);
	p[size] = 0;
	source.setcount(n + size + 1);
	return n;
}

unsigned strlib::find(const char* v, unsigned size) const {
	size_t n = source.getcount();
	auto c = v[0];
	for(size_t i = 0; i < n; i++) {
		if(c == ((char*)source.data)[i]) {
			size_t j = 1;
			auto p = &((char*)source.data)[i];
			for(; j < size; j++)
				if(p[j] != v[j])
					break;
			if(j == size && p[size]==0)
				return i;
		}
	}
	return 0xFFFFFFFF;
}

unsigned strlib::get(const char* v) {
	if(!v || v[0] == 0)
		return 0xFFFFFFFF;
	unsigned s = zlen(v);
	unsigned n = find(v, s);
	if(n == 0xFFFFFFFF)
		n = add(v, s);
	return n;
}

unsigned strlib::get(const char* v, unsigned s) {
	if(!v || !s || v[0] == 0)
		return 0xFFFFFFFF;
	unsigned n = find(v, s);
	if(n == 0xFFFFFFFF)
		n = add(v, s);
	return n;
}