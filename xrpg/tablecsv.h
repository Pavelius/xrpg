#pragma once

struct tablecsvi {
	const char*			id;
	unsigned short		column_count;
	unsigned short		start;
	unsigned short		count;
	const int*			begin() const;
	const int*			end() const;
	bool				parse(const char* p);
	static bool			read(const char* url);
};
