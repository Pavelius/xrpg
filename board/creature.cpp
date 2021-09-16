#include "main.h"
#include "menu.h"

static creaturei*	current;
static variant		param1;
extern variant		menu_result;

void creaturei::clear() {
	memset(this, 0, sizeof(*this));
}

static bool allow_gender(const void* p) {
	return p != bsdata<genderi>::begin();
}

static bool allow_base_race(const void* p) {
	return !((racei*)p)->base.operator bool();
}

static bool allow_subrace(const void* p) {
	return ((racei*)p)->base == param1;
}

static fnmenuallow getallow(const char* id) {
	if(equal("ChooseRace", id))
		return allow_base_race;
	if(equal("ChooseSubRace", id)) {
		param1 = menu_result;
		return allow_subrace;
	}
	if(equal("ChooseGender", id))
		return allow_gender;
	return 0;
}

static void creature_prepare(const char* parent, const char* type) {
	menu_allow = getallow(parent);
}

static void creature_apply(const char* parent, const char* type) {
}

void create_character() {
	menu_prepare = creature_prepare;
	menu_apply = creature_apply;
}