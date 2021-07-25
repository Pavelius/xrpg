#include "main.h"

struct namei {
	const char*		name;
	gender_s		gender;
	role_s			role;
};

BSDATA(namei) = {
	{"Некто", NoGender, Commoner},
	{"Тень", Female, Commoner},
	{"Сова", Female, Commoner},
	{"Хагаст", Male, Enforcer},
	{"Ленни", Male, Enforcer},
	{"Мерл", Male, Enforcer},
	{"Понтис", Male, Enforcer},
	{"Отиак", Male, Enforcer},
	{"Ингрид", Female, Enforcer},
	{"Мабба", Female, Enforcer},
	{"Нельма", Female, Enforcer},
	{"Ребет", Female, Enforcer},
	{"Ронда", Female, Enforcer},
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
	return (short unsigned)an.choose("Каково ваше имя?", 0, interactive, "characters");
}