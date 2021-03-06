#pragma once

typedef decltype(sizeof(0)) size_t;

#define ANREQ(T, R) {(unsigned short)((size_t)&((T*)0)->R), sizeof(T::R)}
#define ANBIT(T, R, B) {(unsigned short)((unsigned)&((T*)0)->R), sizeof(T::R), B}

struct anyreq {
	unsigned short			offset;
	unsigned char			size;
	unsigned char			bit;
	int						get(const void* object) const;
	const char*				gets(const void* object) const;
	void*					ptr(const void* object) const { return (char*)object + offset; }
	void					set(void* object, int value) const;
};