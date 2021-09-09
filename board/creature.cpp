#include "main.h"

void creaturei::clear() {
	memset(this, 0, sizeof(*this));
}

gender_s choose_gender(bool interactive) {
	answers an;
	an.add(Male, getnm("Male"));
	an.add(Female, getnm("Female"));
	return (gender_s)an.choose(getnm("ChooseGender"), 0, interactive, 0);
}

void create_character() {
	auto interactive = true;
	auto gender = choose_gender(interactive);
}