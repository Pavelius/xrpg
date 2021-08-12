#pragma once

namespace codescan {
namespace metrics {
extern int		tabs;
}
bool			isnextline(const char* ps, const char** pv);
void			getstate(const char* p, int origin_y, int& cashe_y, int& maximum_x, int& maximum_y);
}
