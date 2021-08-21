#include "crt.h"
#include "stringbuilder.h"

#pragma once

namespace code {
typedef unsigned pckh;
enum class operation {
	Number, Literal, Symbol,
	Statement, Assign, If, While, Do,
	Add, Sub, Mul, Div, DivPercent, Index,
	Greater, GreaterEqual, Lesser, LesserEqual, Equal, NotEqual,
	LogicalAnd, LogicalOr,
	And, Or, Xor, ShiftLeft, ShiftRight,
	Not, Ref, Der, Negate,
	SizeOf, Return, Call, Point, Break, Continue
};
enum class symf : unsigned char {
	Public, Static, Method, Parameter
};
enum symbol_s : pckh {
	This,
	I32 = 0xFFFFFF00, I16, I8, U32, U16, U8, Void, Bool,
	PI8, PVoid,
	True, False,
	Pointer, Platform, Class,
	SecStr, SecData, SecCode, SecBSS, SecLoc,
	None = 0xFFFFFFFF,
	FirstDef = I32, LastDef = SecLoc
};
struct stringv : public array {
	stringv() : array(1) {}
	pckh				add(const char* v, unsigned len);
	pckh				add(const char* v);
	pckh				find(const char* v, unsigned len) const;
	pckh				find(const char* v) const { return find(v, zlen(v)); }
	const char*			get(pckh v) const;
};
struct ast {
	operation			type;
	pckh				right;
	pckh				left;
	constexpr bool isliteral() const { return type == operation::Literal; }
	constexpr bool issymbol() const { return type == operation::Symbol; }
	constexpr bool isnumber() const { return type == operation::Number; }
};
struct symbol {
	pckh				id = None; // String identificator
	pckh				parent = None; // Parent symbol
	pckh				result = None; // Result symbol type of expression
	unsigned			index = 0; // Position in source file
	unsigned			flags = 0;
	unsigned			local = 0;
	pckh				ast = None; // Abstract syntaxis tree index
	constexpr bool		is(symf v) const { return (flags & (1 << static_cast<int>(v))) != 0; }
	constexpr void		set(symf v) { flags |= 1 << static_cast<int>(v); }
};
class pack {
	pckh				find(operation type, unsigned left, unsigned right) const;
	bool				serial(const char* url, bool write_mode);
public:
	stringv				strings;
	vector<symbol>		symbols;
	vector<ast>			asts;
	explicit operator bool() const { return symbols.getcount() != 0; }
	pckh				add(const char* v) { return strings.add(v); }
	pckh				add(operation type, pckh left, pckh right = 0);
	pckh				addclass(pckh id, pckh result);
	static pack*		addmodule(const char* url);
	pckh				addsym(pckh id, pckh parent, pckh result, unsigned index, unsigned flags);
	static void			addurl(stringbuilder& sb, const char* url, const char* id, const char* ext);
	void				clear();
	void				create(const char* url);
	static pack*		findmodule(const char* url_dup);
	pckh				findstr(const char* v);
	pckh				findsym(pckh id, pckh parent) const;
	void				getast(stringbuilder& sb, unsigned v) const;
	const ast*			getast(pckh v) const;
	static const char*	getmodule(const char* url);
	const char*			getname() const { return (const char*)strings.ptr(0); }
	unsigned			getresult(pckh sym) const;
	pckh				getparent(pckh v) const;
	symbol*				getsymbol(pckh v) const;
	bool				ismethod(pckh sym) const;
	bool				ispointer(pckh sym) const;
	static bool			ispredefined(pckh sym) { return sym >= FirstDef; }
	bool				issymbol(pckh ast) const;
	void				print(stringbuilder& sb);
	void				read(const char* url) { serial(url, false); }
	pckh				reference(pckh v);
	void				setast(pckh sym, pckh ast);
	void				write(const char* url) { serial(url, true); }
};
namespace urls {
extern const char*		project;
extern const char*		projects;
extern const char*		library;
}
}