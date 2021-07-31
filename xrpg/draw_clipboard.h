#pragma once

namespace clipboard {
void			copy(const char* string, long lenght);
bool			isallowpaste();
const char*		paste(long* size = 0);
}
