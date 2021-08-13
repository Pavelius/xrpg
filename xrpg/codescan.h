#include "pointl.h"

#pragma once

namespace codescan {
namespace metrics {
extern int		tabs;
}
enum group_s : unsigned char {
	IllegalSymbol,
	WhiteSpace, Operator, Keyword, Comment,
	Number, String, Identifier
};
int				getindex(const char* p, pointl pos);
const char*		getnext(const char* p, int& x, int& y);
const char*		getnext(const char* p, int& x, int& y, group_s& type);
void			getstate(const char* p, int origin_y, int& cashe_y, pointl& size, int p1, pointl& pp1, int p2, pointl& pp2);
}
