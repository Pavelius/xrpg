#include "crt.h"
#include "pointl.h"

#pragma once

namespace code {
enum group_s : unsigned char {
	IllegalSymbol,
	WhiteSpace, Operator, Keyword, Comment,
	Number, String, Identifier,
	Type,
};
namespace metrics {
extern int				tabs;
}
struct typei {
	const char*			id;
	const char*			parent;
};
struct word {
	const char*			id;
	size_t				size;
	constexpr word(const char* id) : id(id), size(len(id)) {}
	static constexpr const char* end(const char* p) { while(*p) p++; return p; }
	static constexpr unsigned len(const char* p) { return end(p) - p; }
};
typedef slice<word>		worda;
struct lexer {
	const char*			id;
	const char*			extensions;
	worda				keywords, types, operations;
	worda				constants;
};
const word*				find(const worda& source, const char* sym);
const word*				find(const worda& source, const char* sym, unsigned size);
int						getindex(const char* p, pointl pos);
const char*				getnext(const char* p, pointl& pos);
const char*				getnext(const char* p, pointl& pos, group_s& type, const lexer* pk = 0);
void					parse(const char* p, const lexer* pk);
}
