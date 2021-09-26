#include "main.h"

enum action_kind_s {
	SupportAction, MainAction,
};

bool actioni::ismain() const {
	return type == MainAction;
}

BSDATA(actioni) = {
	{"Shield", MainAction},
	{"Retaliate", MainAction},
	{"Evasion"},
	{"Move", MainAction},
	{"Attack", MainAction},
	{"Push", MainAction},
	{"Pull", MainAction},
	{"Heal", MainAction},
	{"DisarmTrap", MainAction},
	{"Loot", MainAction},
	{"Range"},
	{"Target"},
	{"Pierce"},
	{"Bless", MainAction},
	{"Curse", MainAction},
	{"RecoverDiscarded", MainAction},
	{"Experience"},
	{"Level"},
};
assert_enum(actioni, Level)