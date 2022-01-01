#include "crt.h"

#pragma once

template<class T> class agrw {
	T* data;
	unsigned count, maximum;
public:
	agrw* next;
	constexpr agrw(unsigned maximum = 128) : data(0), count(0), maximum(maximum), next(0) {}
	~agrw() { if(data) delete[] data; }
	T* add() {
		auto p = this;
		while(p->next && p->count==p->maximum)
			p = p->next;
		if(p->maximum == p->count) {
			auto n = p->maximum * 2;
			if(n > 256 * 256 * 4)
				n = 256 * 256 * 4;
			p->next = new agrw(n);
			p = p->next;
		}
		if(!p->data)
			p->data = new T[p->maximum];
		return p->data + (p->count++);
	}
	T* begin() { return data; }
	constexpr const T* begin() const { return data; }
	constexpr const T* end() const { return data + count; }
	constexpr unsigned size() const {
		auto r = 0;
		for(auto pp = this; pp; pp = pp->next)
			r += pp->count;
		return r;
	}
};