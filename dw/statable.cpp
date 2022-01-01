#include "main.h"

int statable::last_roll;
result_s statable::last_result;

result_s statable::rollv(int b) {
	if(b < -3)
		b = -3;
	else if(b > 3)
		b = 3;
	last_roll = 2 + rand() % 6 + rand() % 6 + b;
	if(last_roll <= 6)
		last_result = Fail;
	else if(last_roll >= 12)
		last_result = CriticalSuccess;
	else if(last_roll >= 10)
		last_result = Success;
	else
		last_result = PartialSuccess;
	return last_result;
}