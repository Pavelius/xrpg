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
	{"Кварк", Male, Gearhead},
	{"Октан", Male, Gearhead},
	{"Бурбон", Male, Gearhead},
	{"Бинго", Male, Gearhead},
	{"Зиппо", Male, Gearhead},
	{"Дельта", Female, Gearhead},
	{"Иридия", Female, Gearhead},
	{"Лоранга", Female, Gearhead},
	{"Нафта", Female, Gearhead},
	{"Занова", Female, Gearhead},
	{"Данко", Male, Stalker},
	{"Эндел", Male, Stalker},
	{"Фрэнтон", Male, Stalker},
	{"Хамид", Male, Stalker},
	{"Макс", Male, Stalker},
	{"Фелин", Female, Stalker},
	{"Джена", Female, Stalker},
	{"Кейтин", Female, Stalker},
	{"Крин", Female, Stalker},
	{"Тула", Female, Stalker},
	{"Абед", Male, Fixer},
	{"Денрик", Male, Fixer},
	{"Филликс", Male, Fixer},
	{"Джонар", Male, Fixer},
	{"Леодор", Male, Fixer},
	{"Джолиза", Female, Fixer},
	{"Лула", Female, Fixer},
	{"Марлин", Female, Fixer},
	{"Моник", Female, Fixer},
	{"Нова", Female, Fixer},
	{"Финн", Male, DogHandler},
	{"Жони", Male, DogHandler},
	{"Моган", Male, DogHandler},
	{"Монтиак", Male, DogHandler},
	{"Распер", Male, DogHandler},
	{"Энни", Female, DogHandler},
	{"Бри", Female, DogHandler},
	{"Криннель", Female, DogHandler},
	{"Лина", Female, DogHandler},
	{"Софин", Female, DogHandler},
	{"Эристер", Male, Chronicler},
	{"Олиас", Male, Chronicler},
	{"Максим", Male, Chronicler},
	{"Сайлас", Male, Chronicler},
	{"Виктон", Male, Chronicler},
	{"Амара", Female, Chronicler},
	{"Данова", Female, Chronicler},
	{"Джоалин", Female, Chronicler},
	{"Хэннет", Female, Chronicler},
	{"Майри", Female, Chronicler},
	{"Августиан", Male, Boss},
	{"Кристор", Male, Boss},
	{"Максимон", Male, Boss},
	{"Могамин", Male, Boss},
	{"Оскарт", Male, Boss},
	{"Бриктория", Female, Boss},
	{"Элона", Female, Boss},
	{"Ганнет", Female, Boss},
	{"Натара", Female, Boss},
	{"Бристина", Female, Boss},
	{"Динк", Male, Slave},
	{"Филс", Male, Slave},
	{"Хент", Male, Slave},
	{"Март", Male, Slave},
	{"Вайло", Male, Slave},
	{"Алия", Female, Slave},
	{"Эрия", Female, Slave},
	{"Хенни", Female, Slave},
	{"Ким", Female, Slave},
	{"Лин", Female, Slave},
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
	return (short unsigned)an.choose("Каково ваше имя?", 0, interactive, "characters");
}