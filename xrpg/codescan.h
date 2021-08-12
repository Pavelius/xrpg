#pragma once

namespace codescan {
bool			isnextline(const char* ps, const char** pv);
void			getstate(const char* p, int origin_y, int& cashe_y, int& maximum_x, int& maximum_y, int& p1_x, int& p1_y, int& p2_x, int& p2_y, int p1, int p2);
}
