#include "main.h"

struct namei {
	const char*		name;
	gender_s		gender;
	role_s			role;
};

BSDATA(namei) = {
	{"�����", NoGender, Commoner},
	{"����", Female, Commoner},
	{"����", Female, Commoner},
	{"������", Male, Enforcer},
	{"�����", Male, Enforcer},
	{"����", Male, Enforcer},
	{"������", Male, Enforcer},
	{"�����", Male, Enforcer},
	{"������", Female, Enforcer},
	{"�����", Female, Enforcer},
	{"������", Female, Enforcer},
	{"�����", Female, Enforcer},
	{"�����", Female, Enforcer},
};
BSDATAF(namei)

gender_s nameable::getgender() const {
	return bsdata<namei>::elements[name].gender;
}

const char* nameable::getname() const {
	return bsdata<namei>::elements[name].name;
}

short unsigned nameable::choose_name(role_s role, gender_s gender, bool interactive) {
	answers an;
	for(auto& e : bsdata<namei>()) {
		if(role != e.role)
			continue;
		if(gender != NoGender && e.gender != gender)
			continue;
		an.add(&e - bsdata<namei>::elements, "%1 (%2)", e.name, bsdata<genderi>::elements[e.gender].name);
	}
	an.sort();
	return (short unsigned)an.choose("������ ���� ���?", 0, interactive, "characters");
}