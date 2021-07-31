#pragma once

typedef void(*fnevent)();

struct commandi {
	const char*				id;
	fnevent					proc;
	unsigned				key;
	int						image;
};
