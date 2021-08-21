#include "crt.h"

#pragma once

template<typename K, typename V>
struct crecord {
	K			key;
	V			value;
	constexpr bool operator==(K k) const { return key == k; }
};
template<typename K, typename V>
struct cmap : vector<crecord<K, V>> {
	typedef crecord<K, V> element;
	void		add(K& k, V& v) { auto p = add(); p->key = k; p->value = v; }
	element*	add(K& k) { auto p = find(k); if(p) return p; p = vector<crecord<K, V>>::add(); memset(p, 0, sizeof(element)); p->key = k; return p; }
	element*	find(K& k) { for(auto& e : *this) if(e == k) return &e; return 0; }
};