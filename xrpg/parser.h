#include "crt.h"
#include "string.h"

#pragma once

namespace code {
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
typedef slice<rulei>	rulea;
struct corei {
	typedef void (*fnevent)(string st, const rulei& rule, const tokeni& token);
	string				type, id, member, rule, url, comment;
	long long           number = 0;
	const char*			expected;
	fnevent				success;
};
extern const char*		p;
extern corei			core;
extern rulea			this_rules;
void					parse(const char* url_content);
void					skipws();
void					updaterules();
}