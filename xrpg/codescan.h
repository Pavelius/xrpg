#include "crt.h"
#include "pointl.h"

#pragma once

namespace codescan {
enum group_s : unsigned char {
	IllegalSymbol,
	WhiteSpace, Operator, Keyword, Comment,
	Number, String, Identifier,
	Type,
};
namespace metrics {
extern int				tabs;
}
struct lexer {
	struct word {
		const char*		id;
		size_t			size;
		constexpr word(const char* id) : id(id), size(len(id)) {}
		static constexpr const char* end(const char* p) { while(*p) p++; return p; }
		static constexpr unsigned len(const char* p) { return end(p) - p; }
	};
	typedef slice<word> worda;
	const char*			id;
	const char*			extensions;
	worda				keywords, types;
	worda				constants;
	bool				isconstant(const char* sym, unsigned size) const { return find(constants, sym, size) != 0; }
	bool				istype(const char* sym, unsigned size) const { return find(types, sym, size) != 0; }
	static const word*	find(const worda& source, const char* sym, unsigned size);
};
int						getindex(const char* p, pointl pos);
const char*				getnext(const char* p, pointl& pos);
const char*				getnext(const char* p, pointl& pos, group_s& type, const lexer* pk = 0);
}
