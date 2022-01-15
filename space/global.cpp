#include "main.h"

int roll_dices;
int roll_total;
result_s roll_result;

bool groll(int bonus) {
	if(bonus >= 3)
		bonus = 3;
	if(bonus <= -3)
		bonus = -3;
	roll_dices = 2 + (rand() % 6) + (rand() % 6);
	roll_total = roll_dices + bonus;
	roll_result = Fail;
	if(roll_total >= 12)
		roll_total = CriticalSuccess;
	else if(roll_total >= 10)
		roll_total = Success;
	else if(roll_total >= 7)
		roll_total = Partial;
	return roll_result >= Partial;
}