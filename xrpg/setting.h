#include "color.h"

#pragma once

typedef decltype(sizeof(0)) size_t;

namespace setting {
enum type_s : unsigned char {
	Number, Text, Bool,
	Color, Url,
	Control, Radio, Button,
};
typedef bool(*ptest)();
typedef void(*pcall)();
struct reference {
	type_s				type;
	void*				data;
	unsigned			size;
	int					value;
	reference() = default;
	constexpr reference(const reference& v) = default;
	constexpr reference(type_s type, void* data, unsigned size) : type(type), data(data), size(size), value(0) {}
	constexpr reference(type_s type, void* data, unsigned size, int value) : type(type), data(data), size(size), value(value) {}
	template<class T> constexpr reference(T& v) : reference(Number, &v, sizeof(v)) {}
	constexpr reference(const char*& v) : reference(Text, &v, sizeof(v)) {}
	constexpr reference(const char*& v, type_s type) : reference(type, &v, sizeof(v)) {}
	constexpr reference(bool& v) : reference(Bool, &v, sizeof(v)) {}
	constexpr reference(color& v) : reference(Color, &v, sizeof(v)) {}
	constexpr reference(pcall v) : reference(Button, (void*)v, sizeof(v)) {}
	long				get() const;
	void*				getptr() const { return data; }
	bool				iszero() const { return get() == 0; }
	void				set(long v) const;
};
struct element {
	const char*			name;
	reference			var;
	int					param;
	ptest				test; // Test visibility
};
template<class T>
class list {
	const T*			data;
	unsigned			count;
public:
	const T*			begin() const { return data; }
	const T*			end() const { return data + count; }
	list() = default;
	template<class M, unsigned N> constexpr list(M(&data)[N]) : data(data), count(N) {};
};
typedef list<element> elementa;
struct header {
	const char*			division;
	const char*			page;
	const char*			group;
	elementa			elements;
	ptest				visible;
	header*				next;
	header(const char* division, const char* page, const char* group, const elementa& elements, ptest visible = 0);
	static header*		first;
};
}