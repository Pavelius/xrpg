#include "main.h"

static character* current_character;

character* gamei::getplayer() {
	return current_character;
}

static role_s choose_role() {
	varianta source;
	source.select(bsdata<rolei>::source);
	return (role_s)source.choose("Какова ваша роль?", 0, true, "new_character").value;
}

static void choose_attribute(character& e, int score) {
	char temp[260]; stringbuilder sb(temp);
	answers an;
	while(true) {
		auto current = e.getattributepoints();
		auto left = score - current;
		an.clear();
		if(left > 0)
			an.add(1, "Повысить характеристику");
		if(current > 2 * 4)
			an.add(-1, "Понизить характеристику");
		if(left == 0)
			an.add(100, "Готово");
		sb.clear();
		sb.add("У вас есть [%1i] очков, которые необходимо распределить на характеристики.", left);
		auto type = an.choose(temp, 0, true, "skills");
		if(type == 100)
			break;
		an.clear();
		for(auto& ei : bsdata<attributei>()) {
			auto v = (attribute_s)(&ei - bsdata<attributei>::elements);
			auto value = e.get(v) + type;
			auto maximum = e.getmaximum(v);
			if(value > maximum || value < 2)
				continue;
			an.add((long)&ei, szt(bsdata<attributei>::elements[v].id));
		}
		auto id = (attribute_s)variant((void*)an.choose(0, 0, true, "skills")).value;
		e.set(id, e.get(id) + type);
	}
}

static void choose_skills(character& e, int score, int maximum) {
	char temp[260]; stringbuilder sb(temp);
	answers an;
	while(true) {
		auto current = e.getskillpoints();
		auto left = score - current;
		an.clear();
		if(left > 0)
			an.add(1, "Повысить навык");
		if(current > 1)
			an.add(-1, "Понизить навык");
		if(left == 0)
			an.add(100, "Готово");
		sb.clear();
		sb.add("У вас есть [%1i] очков, которые необходимо распределить на ваши навыки.", left);
		auto type = an.choose(temp, 0, true, "skills");
		if(type == 100)
			break;
		an.clear();
		for(auto& ei : bsdata<skilli>()) {
			auto v = (skill_s)(&ei - bsdata<skilli>::elements);
			auto value = e.get(v) + type;
			if(value > maximum || value < e.getminimum(v))
				continue;
			an.add((long)&ei, szt(bsdata<skilli>::elements[v].id));
		}
		auto id = (skill_s)variant((void*)an.choose(0, 0, true, "skills")).value;
		e.set(id, e.get(id) + type);
	}
}

void character::clear() {
	memset(this, 0, sizeof(*this));
}

int	character::getminimum(skill_s v) const {
	if(kind.type == Role) {
		auto& ei = bsdata<rolei>::elements[getrole()];
		if(ei.skill == v)
			return 1;
	}
	return 0;
}

int	character::getmaximum(attribute_s v) const {
	if(kind.type != Role)
		return 0;
	auto& e = bsdata<rolei>::elements[kind.value];
	return (e.attribute == v) ? 5 : 4;
}

void character::create_new() {
	static const char* character_panel[] = {"CharacterSheet", 0};
	draw::form.window = getcommand("CharacterSheet");
	draw::form.commands = character_panel;
	current_character = bsdata<character>::add();
	current_character->clear();
	current_character->kind.setvariant(Role, choose_role());
	current_character->setname(choose_name(current_character->getrole(), NoGender, true));
	current_character->create(current_character->getrole());
	choose_attribute(*current_character, 14);
	choose_skills(*current_character, 10, 3);
}

role_s character::getrole() const {
	switch(kind.type) {
	case Role: return (role_s)kind.value;
	default: return Commoner;
	}
}