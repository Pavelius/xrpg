#include "parser.h"
#include "pointl.h"

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
typedef vector<string>	worda;
struct lexer {
	const char*			id;
	const char*			extensions;
	rulea				grammar;
	slice<string>		standart_classes;
	worda				keywords, classes, operations;
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
