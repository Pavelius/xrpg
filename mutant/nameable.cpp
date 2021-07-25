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
	{"�����", Male, Gearhead},
	{"�����", Male, Gearhead},
	{"������", Male, Gearhead},
	{"�����", Male, Gearhead},
	{"�����", Male, Gearhead},
	{"������", Female, Gearhead},
	{"������", Female, Gearhead},
	{"�������", Female, Gearhead},
	{"�����", Female, Gearhead},
	{"������", Female, Gearhead},
	{"�����", Male, Stalker},
	{"�����", Male, Stalker},
	{"�������", Male, Stalker},
	{"�����", Male, Stalker},
	{"����", Male, Stalker},
	{"�����", Female, Stalker},
	{"�����", Female, Stalker},
	{"������", Female, Stalker},
	{"����", Female, Stalker},
	{"����", Female, Stalker},
	{"����", Male, Fixer},
	{"������", Male, Fixer},
	{"�������", Male, Fixer},
	{"������", Male, Fixer},
	{"������", Male, Fixer},
	{"�������", Female, Fixer},
	{"����", Female, Fixer},
	{"������", Female, Fixer},
	{"�����", Female, Fixer},
	{"����", Female, Fixer},
	{"����", Male, DogHandler},
	{"����", Male, DogHandler},
	{"�����", Male, DogHandler},
	{"�������", Male, DogHandler},
	{"������", Male, DogHandler},
	{"����", Female, DogHandler},
	{"���", Female, DogHandler},
	{"��������", Female, DogHandler},
	{"����", Female, DogHandler},
	{"�����", Female, DogHandler},
	{"�������", Male, Chronicler},
	{"�����", Male, Chronicler},
	{"������", Male, Chronicler},
	{"������", Male, Chronicler},
	{"������", Male, Chronicler},
	{"�����", Female, Chronicler},
	{"������", Female, Chronicler},
	{"�������", Female, Chronicler},
	{"������", Female, Chronicler},
	{"�����", Female, Chronicler},
	{"���������", Male, Boss},
	{"�������", Male, Boss},
	{"��������", Male, Boss},
	{"�������", Male, Boss},
	{"������", Male, Boss},
	{"���������", Female, Boss},
	{"�����", Female, Boss},
	{"������", Female, Boss},
	{"������", Female, Boss},
	{"��������", Female, Boss},
	{"����", Male, Slave},
	{"����", Male, Slave},
	{"����", Male, Slave},
	{"����", Male, Slave},
	{"�����", Male, Slave},
	{"����", Female, Slave},
	{"����", Female, Slave},
	{"�����", Female, Slave},
	{"���", Female, Slave},
	{"���", Female, Slave},
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
		an.add(&e - bsdata<namei>::elements, e.name);
	}
	//an.sort();
	return (short unsigned)an.choose("������ ���� ���?", 0, interactive, "characters");
}