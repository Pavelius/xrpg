#pragma once

typedef void(*fnevent)();

struct command {
	const char*				id;
	fnevent					proc;
	unsigned				key;
	int						image;
	constexpr explicit operator bool() const { return id != 0; }
	const command*			find(const char* v) const;
	const command*			find(unsigned v) const;
};