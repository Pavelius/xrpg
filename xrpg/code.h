#include "crt.h"
#include "pointl.h"
#include "string.h"

#pragma once

namespace code {
namespace metrics {
extern int				tabs;
}
enum group_s : unsigned char {
	IllegalSymbol,
	WhiteSpace, Operator, Keyword, Comment,
	Number, String, Identifier, Type,
	BlockBegin, BlockEnd, IndexBegin, IndexEnd, ExpressionBegin, ExpressionEnd,
};
enum class flag : unsigned char {
	Variable, Condition, Repeat, ComaSeparated, PointSeparated, Stop,
};
struct corei {
	string				type, id, member, rule, url, comment;
	long long           number = 0;
};
extern corei			core;
struct typei {
	const char*			id;
	const char*			result;
	const char*			url;
	explicit constexpr operator bool() const { return id != 0; }
	static typei*		add(string id, const char* result = 0, const char* url = 0);
	void				clear();
	static typei*		find(string id, const char* url);
};
struct memberi : typei {
	const char*			parent;
	explicit constexpr operator bool() const { return id != 0; }
	static memberi*		add(string id, const char* result, const char* parent, const char* url);
	void				clear();
	static memberi*		find(string id, const char* url);
};
struct tokeni {
	const char*			id;
	unsigned			flags;
	const struct rulei*	rule;
	constexpr tokeni() : id(0), flags(), rule(0) {}
	constexpr tokeni(const char* p) : id(p), flags(), rule(0) {
		while(*p) {
			if(*p == '\\') {
				id = p + 1;
				break;
			} else if(p[0] == ',' && p[1] == ' ') {
				set(flag::ComaSeparated); p++;
			} else if(p[0] == '.' && p[1] == ' ') {
				set(flag::PointSeparated); p++;
			} else if(*p == '^')
				set(flag::Stop);
			else if(*p == '.')
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
	const char*			name;
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
