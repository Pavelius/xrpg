#include "serializer.h"

int	serializer::node::getlevel() const {
	auto result = 0;
	for(auto p = parent; p; p = p->parent)
		result++;
	return result;
}