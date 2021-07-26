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

void statable::create(role_s role) {
	auto& ei = bsdata<rolei>::elements[role];
	for(auto id = Strenght; id <= Empathy; id = (attribute_s)(id + 1))
		set(id, 2);
	set(ei.skill, 1);
}