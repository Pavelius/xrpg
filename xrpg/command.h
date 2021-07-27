#pragma once

typedef void(*fnevent)();

struct commandi {
	const char*				id;
	fnevent					proc;
	const char*				name;
	const char*				text;
};
