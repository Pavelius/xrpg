#include "crt.h"

#pragma once

struct menui {
	struct resulti {
		const char*		next;
		fnevent			proc;
	};
	const char*			parent;
	const char*			name;
	resulti				result;
	const char*			text;
};
