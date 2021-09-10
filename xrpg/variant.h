#include "crt.h"
#include "stringbuilder.h"

#pragma once

#define VKIND(T, V) template<> static constexpr variant_s variant::kind<T>() { return V; }

enum variant_s : unsigned char;
struct bsreq;

struct varianti {
	const char*			id;
	const bsreq*		metadata;
	array*				source;
};
union variant{
	unsigned char		uc[4];
	unsigned			u;
	struct {
		unsigned short	value;
		variant_s		type;
	};
	constexpr variant() : u(0) {}
	constexpr variant(variant_s t, unsigned short n) : value(n), type(t) {}
	constexpr variant(int u) : u(u) {}
	template<class T> static constexpr variant_s kind();
	template<class T> variant(T* v) : variant((const void*)v) {}
	template<class T> constexpr variant(T v) : variant(kind<T>(), v) {}
	constexpr operator int() const { return u; }
	constexpr explicit operator bool() const { return u != 0; }
	constexpr bool operator==(const variant& v) const { return u == v.u; }
	constexpr bool operator!=(const variant& v) const { return u != v.u; }
	template<class T> operator T*() const { return (T*)((kind<T>() == type) ? getpointer() : 0); }
	void				clear() { u = 0; }
	const char*			getdescription() const;
	int					getindex(int t) const { return (type == t) ? value : 0; }
	void				getinfo(stringbuilder& sb) const;
	void*				getpointer() const { return bsdata<varianti>::elements[type].source->ptr(value); }
	const char*			getname() const;
	void				paint() const;
	void				setvariant(variant_s t, unsigned short v) { type = t; value = v; }
};
typedef sliceu<variant> variants;
template<> variant::variant(const char* v);
template<> variant::variant(const void* v);
