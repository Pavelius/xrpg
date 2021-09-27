#include "main.h"

bool actioni::ismain() const {
	return type == MainAction;
}

BSDATA(actioni) = {
	{"Shield", MainAction},
	{"Retaliate", MainAction},
	{"Evasion", MainAction},
	{"Move", MainAction},
	{"Attack", MainAction},
	{"Push", MainAction},
	{"Pull", MainAction},
	{"Heal", MainAction},
	{"DisarmTrap", MainAction},
	{"Loot", MainAction},
	{"Range", Modifier},
	{"Target", Modifier},
	{"Pierce", Modifier},
	{"Bless", MainAction},
	{"Curse", MainAction},
	{"RecoverDiscarded", MainAction},
	{"Experience", Modifier},
	{"Level", Modifier},
};
assert_enum(actioni, Level)