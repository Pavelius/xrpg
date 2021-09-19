#pragma once

enum reference_s : unsigned char {
	None,
	Types, Text, Number,
	ID, Parent, Param,
};

struct reference {
	unsigned			id;
	unsigned			parent;
	unsigned			param;
	unsigned			get(unsigned req) const;
	unsigned			getindex() const;
	const char*			gets(unsigned req) const;
	void				set(unsigned req);
	void				set(unsigned req, const char* v);
	void				set(unsigned req, unsigned v);
};
struct valuei {
	unsigned			parent;
	unsigned			requisit;
	unsigned			value;
	void				clear();
};
extern const char*		getstr(unsigned v);
extern unsigned			getidx(const char* v);

