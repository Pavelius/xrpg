#include "crt.h"
#include "stringbuilder.h"

#pragma once

struct bsreq;
struct varianti {
	const char*			id;
	const bsreq*		metadata;
	array*				source;
	unsigned			names[2];
};
class variant {
	union {
		unsigned char	c[4];
		unsigned		u;
	};
public:
	constexpr variant() : u(0) {}
	constexpr variant(unsigned char t, unsigned char n) : c{n, 0, 0, t} {}
	constexpr variant(int u) : u(u) {}
	variant(const char* v);
	variant(const void* v);
	constexpr operator int() const { return u; }
	constexpr explicit operator bool() const { return u != 0; }
	constexpr bool operator==(const variant& v) const { return u == v.u; }
	constexpr bool operator!=(const variant& v) const { return u != v.u; }
	template<class T> operator T*() const { return (T*)((T::kind == c[3]) ? getpointer() : 0); }
	void				clear() { u = 0; }
	int					getindex(int t) const { return (getkind() == t) ? getvalue() : 0; }
	void				getinfo(stringbuilder& sb) const;
	void*				getpointer() const { return bsdata<varianti>::elements[c[3]].source->ptr(u & 0xFFFFFF); }
	constexpr int		getkind() const { return c[3]; }
	const char*			getname() const;
	constexpr int		getvalue() const { return u & 0xFFFFFF; }
	void				paint() const;
	void				setvariant(unsigned char t, unsigned char v) { c[3] = t; c[0] = v; }
};