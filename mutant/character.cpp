#include "main.h"

static character* current_character;

character* gamei::getplayer() {
	return current_character;
}

static role_s choose_role() {
	varianta source;
	source.select(bsdata<rolei>::source);
	return (role_s)source.choose("?????? ???? ?????", 0, true, "new_character").value;
}

static void choose_attribute(character& e, int score) {
	char temp[260]; stringbuilder sb(temp);
	answers an;
	while(true) {
		auto current = e.getattributepoints();
		auto left = score - current;
		an.clear();
		if(left > 0)
			an.add(1, "???????? ??????????????");
		if(current > 2 * 4)
			an.add(-1, "???????? ??????????????");
		if(left == 0)
			an.add(100, "??????");
		sb.clear();
		sb.add("? ??? ???? [%1i] ?????, ??????? ?????????? ???????????? ?? ??????????????.", left);
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
			an.add((long)&ei, getnm(bsdata<attributei>::elements[v].id));
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
			an.add(1, "???????? ?????");
		if(current > 1)
			an.add(-1, "???????? ?????");
		if(left == 0)
			an.add(100, "??????");
		sb.clear();
		sb.add("? ??? ???? [%1i] ?????, ??????? ?????????? ???????????? ?? ???? ??????.", left);
		auto type = an.choose(temp, 0, true, "skills");
		if(type == 100)
			break;
		an.clear();
		for(auto& ei : bsdata<skilli>()) {
			auto v = (skill_s)(&ei - bsdata<skilli>::elements);
			auto value = e.get(v) + type;
			if(value > maximum || value < e.getminimum(v))
				continue;
			an.add((long)&ei, getnm(bsdata<skilli>::elements[v].id));
		}
		auto id = (skill_s)variant((void*)an.choose(0, 0, true, "skills")).value;
		e.set(id, e.get(id) + type);
	}
}

static void choose_talent(character& e, bool filter_by_role) {
	char temp[260]; stringbuilder sb(temp);
	answers an;
	for(auto& ei : bsdata<talenti>()) {
		if(filter_by_role) {
			if(ei.role != e.getrole())
				continue;
		}
		an.add((long)&ei, getnm(ei.id));
	}
	auto id = (talent_s)variant((void*)an.choose(getnm("ChooseTalent"), 0, true, "skills")).value;
	e.set(id);
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
	current_character = bsdata<character>::add();
	current_character->clear();
	current_character->kind.setvariant(Role, choose_role());
	current_character->setname(choose_name(current_character->getrole(), NoGender, true));
	current_character->create(current_character->getrole());
	choose_attribute(*current_character, 14);
	choose_skills(*current_character, 10, 3);
	choose_talent(*current_character, true);
}

role_s character::getrole() const {
	switch(kind.type) {
	case Role: return (role_s)kind.value;
	default: return Commoner;
	}
}