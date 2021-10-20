#include "crt.h"

#pragma once

#define	BSREQ(fn) {#fn, FO(data_type, fn),\
sizeof(meta_size<decltype(data_type::fn)>::value),\
sizeof(data_type::fn),\
meta_count<decltype(data_type::fn)>::value,\
bsmeta<meta_decoy<decltype(data_type::fn)>::value>::meta,\
meta_kind<decltype(data_type::fn)>::value,\
bsdata<meta_decoy<decltype(data_type::fn)>::value>::source_ptr}
#define	BSCST(fn, K, TM, TE, C) {#fn, FO(data_type, fn), sizeof(TM), sizeof(data_type::fn), C, bsmeta<TM>::meta, K, bsdata<TE>::source_ptr}
#define	BSDST(fn, TE) BSCST(fn, KindDSet, decltype(data_type::fn)::data_type, TE, 1)
#define	BSFLG(fn, T) {#fn, FO(data_type, fn), sizeof(data_type::fn), sizeof(data_type::fn), 1, bsmeta<T>::meta, KindFlags, bsdata<T>::source_ptr}
#define BSMETA(e) template<> const bsreq bsmeta<e>::meta[]
#define BSINF(N, T) {#N, 0, sizeof(T), sizeof(T), 1, bsmeta<T>::meta, KindScalar, bsdata<T>::source_ptr}

// Basic metadata types
enum bstype_s : unsigned char {
	KindNoType,
	KindNumber, KindText, KindScalar, KindEnum, KindReference,
	KindADat, KindSlice, KindFlags, KindDSet
};
// Metadata field descriptor
struct bsreq {
	const char*				id; // field identifier
	unsigned				offset; // offset from begin of class or object
	unsigned				size; // size of single element
	unsigned				lenght; // total size in bytes of all field (array has size*count)
	unsigned				count; // count of elements
	const bsreq*			type; // metadata of element
	bstype_s				subtype; // metadata subtype
	array*					source; // data source for enumerators
	constexpr explicit operator bool() const { return id != 0; }
	void*					dereference(const void* data, const bsreq** result) const;
	bool					equal(const void* v1, const void* v2) const;
	const bsreq*			find(const char* name) const;
	const bsreq*			find(const char* name, unsigned count) const;
	const bsreq*			find(const char* name, const bsreq* type) const;
	int						findenum(const char* name) const;
	long					get(const void* p) const;
	const char*				get(const void* p, char* result, const char* result_max) const;
	const char*				gets(const void* p) const;
	const bsreq*			getname() const;
	const char*				getmetaname() const; // Extern function
	bool					is(bstype_s v) const { return subtype == v; }
	bool					issimple() const { return is(KindNumber) || is(KindText); }
	bool					match(const void* p, const char* name) const;
	char*					ptr(const void* data) const { return (char*)data + offset; }
	char*					ptr(const void* data, int index) const { return (char*)data + offset + index * size; }
	void*					ptr(const void* data, const char* url, const bsreq** result) const;
	static void				read(const char* url);
	void					set(const void* p, long value) const;
	bool					write(const char* url, void* object) const;
};
NOBSDATA(bsreq)
// Abstract metadata class
template<typename T> struct bsmeta {
	typedef T				data_type;
	static const bsreq		meta[];
};
template<> struct bsmeta<unsigned char> : bsmeta<int> {};
template<> struct bsmeta<char> : bsmeta<int> {};
template<> struct bsmeta<unsigned short> : bsmeta<int> {};
template<> struct bsmeta<short> : bsmeta<int> {};
template<> struct bsmeta<unsigned> : bsmeta<int> {};
template<> struct bsmeta<bool> : bsmeta<int> {};
// Untility structures
template<typename T, T v> struct static_value { static constexpr T value = v; };
template<int v> struct static_int : static_value<int, v> {};
// Get array elments
template<class T> struct meta_count : static_int<1> {};
template<class T, unsigned N> struct meta_count<T[N]> : static_int<N> {};
template<class T> struct meta_count<T[]> : static_int<0> {};
template<class T, unsigned N> struct meta_count<adat<T, N>> : static_int<N> {};
// Get base size
template<class T> struct meta_size : meta_decoy<T> {};
template<class T> struct meta_size<T*> { typedef T* value; };
template<class T> struct meta_size<const T*> { typedef T* value; };
template<class T, unsigned N> struct meta_size<T[N]> : meta_size<T> {};
template<class T> struct meta_size<T[]> : meta_size<T> {};
template<class T> struct meta_size<sliceu<T>> : meta_size<T> {};
// Get kind
template<class T> struct meta_kind : static_value<bstype_s, __is_enum(T) ? KindEnum : KindScalar> {};
template<> struct meta_kind<const char*> : static_value<bstype_s, KindText> {};
template<> struct meta_kind<char> : static_value<bstype_s, KindNumber> {};
template<> struct meta_kind<short> : static_value<bstype_s, KindNumber> {};
template<> struct meta_kind<int> : static_value<bstype_s, KindNumber> {};
template<> struct meta_kind<unsigned char> : static_value<bstype_s, KindNumber> {};
template<> struct meta_kind<unsigned short> : static_value<bstype_s, KindNumber> {};
template<> struct meta_kind<unsigned int> : static_value<bstype_s, KindNumber> {};
template<> struct meta_kind<bool> : static_value<bstype_s, KindNumber> {};
template<class T> struct meta_kind<T*> : static_value<bstype_s, KindReference> {};
template<class T> struct meta_kind<T**> {}; // Not allowed complex pointer
template<class T> struct meta_kind<const T*> : static_value<bstype_s, KindReference> {};
template<class T> struct meta_kind<const T> : static_value<bstype_s, meta_kind<T>::value> {};
template<class T> struct meta_kind<sliceu<T>> : static_value<bstype_s, KindSlice> {};
template<class T, unsigned N> struct meta_kind<T[N]> : static_value<bstype_s, meta_kind<T>::value> {};
template<class T, unsigned N> struct meta_kind<adat<T, N>> : static_value<bstype_s, KindADat> {};
//template<class T> struct meta_kind<std::initializer_list<T>> : static_value<bstype_s, KindList> {};
//template<class T, class DT> struct meta_kind<cflags<T, DT>> : static_value<bstype_s, KindCFlags> {};