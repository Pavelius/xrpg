#include "crt.h"
#include "pointl.h"

#pragma once

namespace codescan {
enum group_s : unsigned char {
	IllegalSymbol,
	WhiteSpace, Operator, Keyword, Comment,
	Number, String, Identifier
};
namespace metrics {
extern int		tabs;
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
	worda				keywords;
	const word*			find(const char* sym, unsigned size) const;
};
int						getindex(const char* p, pointl pos);
const char*				getnext(const char* p, pointl& pos);
const char*				getnext(const char* p, pointl& pos, group_s& type, const lexer* pk = 0);
void					getstate(const char* p, int origin_y, int& cashe_y, pointl& size, int p1, pointl& pp1, int p2, pointl& pp2);
}
