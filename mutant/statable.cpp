#include "main.h"

int statable::getskillpoints() const {
	auto r = 0;
	for(auto& e : skills)
		r += e;
	return r;
}

int statable::getattributepoints() const {
	auto r = 0;
	for(auto& e : attributes)
		r += e;
	return r;
}