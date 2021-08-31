#include "crt.h"
#include "stringbuilder.h"
#include "string.h"

#pragma once

namespace code {
typedef unsigned long pckh;
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
	Pointer = 0xFFFFFF00, Platform, Class,
	SecStr, SecData, SecCode, SecBSS, SecLoc,
	None = 0xFFFFFFFF
};
struct symbol;
class pack {
	array				strings;
	array				symbols;
	array				asts;
	pckh				find(operation type, pckh left, pckh right) const;
	bool				serial(const char* url, bool write_mode);
public:
	class record {
		pckh			r1, r2;
		class element {
			pckh		v;
		public:
			constexpr element(unsigned v) : v(v) {}
			constexpr bool operator==(const element& e) const { return v == e.v; }
			constexpr bool operator!=(const element& e) const { return v != e.v; }
			constexpr pckh operator*() { return v; }
			constexpr void operator++() { v++; }
		};
	public:
		constexpr record(pckh r1, pckh r2) : r1(r1), r2(r2) {}
		constexpr unsigned operator*(unsigned v) const { return v; }
		element			begin() const { return r1; }
		element			end() const { return r2; }
	};
	explicit operator bool() const { return symbols.getcount() != 0; }
	pckh				add(const char* v);
	pckh				add(const string& v) { return add(v.begin(), v.size); }
	pckh				add(const char* v, unsigned s);
	pckh				add(operation type, pckh left, pckh right = 0);
	pckh				addclass(pckh id, pckh result);
	void				addclasses(slice<string> source);
	static pack*		addmodule(const char* url);
	pckh				addsym(pckh id, pckh parent, pckh result, unsigned index, unsigned flags);
	void				clear();
	void				create(const char* url);
	pckh				find(const char* v) const;
	pckh				find(const char* v, unsigned s) const;
	pckh				find(pckh id, pckh parent) const;
	static pack*		findmodule(const char* url);
	record				getsymbols() const;
	unsigned			getflags(pckh vm) const;
	static const char*	getmodule(const char* url);
	const char*			getname() const { return (const char*)strings.ptr(0); }
	const char*			getname(pckh v) const;
	pckh				getparent(pckh v) const;
	pckh				getresult(pckh v) const;
	void				parse(const char* url_content);
	void				read(const char* url) { serial(url, false); }
	pckh				reference(pckh v);
	void				write(const char* url) { serial(url, true); }
};
extern const char*		p;
void					skipws();
extern pack*			this_pack;
namespace urls {
extern const char*		project;
extern const char*		projects;
extern const char*		library;
}
}