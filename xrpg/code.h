#include "crt.h"
#include "pointl.h"

#pragma once

namespace code {
enum group_s : unsigned char {
	IllegalSymbol,
	WhiteSpace, Operator, Keyword, Comment,
	Number, String, Identifier,	Type,
	BlockBegin, BlockEnd, IndexBegin, IndexEnd, ExpressionBegin, ExpressionEnd,
};
enum class flag : unsigned char {
	Variable, Condition, Repeat
};
enum class error : unsigned char {
	ExpectedP1,
};
namespace metrics {
extern int				tabs;
}
struct typei {
	const char*			id;
	const char*			value;
	const char*			url;
	explicit constexpr operator bool() const { return id != 0; }
	void				clear();
};
struct memberi {
	const char*			id;
	const char*			type;
	const char*			result;
	const char*			url;
	explicit constexpr operator bool() const { return id != 0; }
	void				clear();
};
struct tokeni {
	const char*			id;
	unsigned			flags;
	const struct rulei*	rule;
	constexpr tokeni() : flags(), id(0), rule(0) {}
	constexpr tokeni(const char* p) : flags(), id(p), rule(0) {
		while(*p) {
			if(*p == '\\') {
				id = p + 1;
				break;
			} else if(*p == '.')
				set(flag::Repeat);
			else if(*p == '%')
				set(flag::Variable);
			else if(*p == '?')
				set(flag::Condition);
			else {
				id = p;
				break;
			}
			p++;
		}
	}
	constexpr explicit operator bool() const { return id != 0; }
	constexpr bool		is(flag v) const { return (flags & (1 << (int)v)) != 0; }
	void				parse() const;
	constexpr void		set(flag v) { flags |= 1 << (int)v; }
};
typedef tokeni			tokena[16];
struct rulei {
	tokeni				name;
	tokena				tokens;
	fnevent				special;
	void				parse() const;
};
typedef slice<rulei>	rulea;
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
void					parse(const char* url, const char* p, const lexer* pk);
}
void					initialize_codeview();
void					initialize_codetree();
void					initialize_complex_grammar();
void					update_codetree();
