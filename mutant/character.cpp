#include "main.h"

static character* current_character;

static role_s choose_role() {
	varianta source;
	source.select(bsdata<rolei>::source);
	return (role_s)source.choose("Какова ваша роль?", 0, true, "new_character").getvalue();
}

void character::create_new() {
	auto p = bsdata<character>::add();
	p->setvariant(Role, choose_role());
	p->setname(choose_name(p->getrole(), NoGender, true));
}

role_s character::getrole() const {
	switch(getkind()) {
	case Role: return (role_s)getvalue();
	default: return Commoner;
	}
}