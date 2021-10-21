#pragma once

enum season_s : unsigned char {
	Spring, Summer, Autum, Winter
};
struct calendari {
	const char*		id;
	season_s		season;
	int				season_part;
};
struct monthparti {
	const char*		id;
};
struct seasoni {
	const char*		id;
};