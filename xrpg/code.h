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
struct corei {
	typedef void (*fnevent)(string st, const rulei& rule, const tokeni& token);
	string				type, id, member, rule, url, comment;
	long long           number = 0;
	const char*			expected;
	fnevent				success;
};
extern corei			core;
typedef slice<rulei>	rulea;
typedef vector<string>	worda;
struct lexer {
	const char*			id;
	const char*			extensions;
	rulea				grammar;
	slice<string>		standart_classes;
	worda				keywords, classes, operations;
	void				addclass(const char* id, int size, bool type_unsigned = false);
	void				initialize();
	void				setgrammar() const;
};
const string*			find(const worda& source, const string& v);
int						getindex(const char* p, pointl pos);
const char*				getnext(const char* p, pointl& pos);
const char*				getnext(const char* p, pointl& pos, group_s& type, const lexer* pk = 0);
}
void					initialize_codeview();
void					initialize_codetree();
void					initialize_lexers();
void					update_codetree();
