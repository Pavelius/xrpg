#include "main.h"

BSDATA(movei) = {
	{"HackAndSlash", Strenght},
	{"Volley", Dexterity},
	{"DefyDanger", Dexterity},
	{"DefyPoison", Constitution},
	{"Defend", Constitution},
	{"SpoutLore", Intellegence},
	{"DiscernRealities", Wisdow},
	{"Parley", Charisma},
	{"Aid"},
	{"Interfere"},
	{"LastBreath"},
	{"Encumbrance"},
	{"MakeCamp"},
	{"TakeWatch", Wisdow},
	{"UndertakeAPerilousJourney", Wisdow},
	{"LevelUp"},
	{"EndOfSession"},
	{"Carouse", Charisma},
	{"Supply", Charisma},
	{"Recover"},
	{"Recruit"},
	{"OutstandingWarrants", Charisma},
	{"Bolster"},
};
assert_enum(movei, Bolster)