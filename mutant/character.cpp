#include "main.h"

static character* current_character;

character* gamei::getplayer() {
	return current_character;
}

static role_s choose_role() {
	varianta source;
	source.select(bsdata<rolei>::source);
	return (role_s)source.choose("Какова ваша роль?", 0, true, "new_character").getvalue();
}

static void choose_skills(character& e, int score) {
	char temp[260]; stringbuilder sb(temp);
	answers an;
	while(true) {
		auto current = e.getskillpoints();
		auto left = score - current;
		an.clear();
		if(left > 0)
			an.add(1, "Повысить характеристику");
		if(current > 0)
			an.add(-1, "Понизить характеристику");
		if(left == 0)
			an.add(100, "Готово");
		sb.clear();
		sb.add("У вас есть [%1i] очков, которые необходимо распределить на ваши навыки.", left);
		auto type = an.choose(temp, 0, true, "skills");
		if(type == 100)
			break;
		an.clear();
		for(auto skill = Endure; skill <= Intimidate; skill = (skill_s)(skill + 1)) {
			auto value = e.get(skill);
			if(type > 0) {
				if(!value && left < 2)
					continue;
				if(value >= 4)
					continue;
			} else {
				if(!value)
					continue;
			}
			an.add(skill, bsdata<skilli>::elements[skill].name);
		}
		auto skill = (skill_s)an.choose(0, 0, true, "skills");
		auto value = e.get(skill);
		auto maximum = 4;
		auto minimum = 2;
		value += type;
		if(value < minimum)
			value = minimum;
		if(value > maximum)
			value = maximum;
		e.set(skill, value);
	}
}

void character::create_new() {
	current_character = bsdata<character>::add();
	current_character->setvariant(Role, choose_role());
	current_character->setname(choose_name(current_character->getrole(), NoGender, true));
	choose_skills(*current_character, 14);
}

role_s character::getrole() const {
	switch(getkind()) {
	case Role: return (role_s)getvalue();
	default: return Commoner;
	}
}