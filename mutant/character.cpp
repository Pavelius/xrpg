#include "main.h"

static character* current_character;

void character::choose_role() {
	varianta source;
	source.select(bsdata<rolei>::source);
	auto v = source.choose("������ ���� ����?", 0, true, "new_character");
}

