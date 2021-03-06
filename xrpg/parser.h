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
	fnevent				special, before;
	void				parse() const;
};
typedef slice<rulei>	rulea;
struct memberlisti {
	virtual void		add(int type, string name) = 0;
};
struct corei {
	string				type, id, member, rule, url, comment;
	const char*			context;
	long long           number;
	const char*			expected;
	memberlisti*		memberlist;
	void				clear();
};
struct errori {
	const char*			p;
	const char*			expected;
	const char*			format;
	void				clear();
};
extern const char*		p;
extern errori			error;
extern corei			core;
extern rulea			this_rules;
void					identifier();
void					literal();
void					number();
bool					parse(const char* url_content, const char* rule_name = 0);
void					skipws();
void					updaterules();
}