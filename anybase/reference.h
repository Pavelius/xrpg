#pragma once

enum reference_s : unsigned char {
	None,
	Types, Text, Number,
	ID, 
};

struct reference {
	unsigned			id;
	unsigned			parent;
	unsigned			param;
	int					get(unsigned req) const { return 0; }
};
struct valuei {
	unsigned			parent;
	unsigned			requisit;
	unsigned			value;
};
extern const char*		getstr(unsigned v);
extern unsigned			getidx(const char* v);

