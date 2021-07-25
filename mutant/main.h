#include "answers.h"
#include "datamap.h"
#include "dataset.h"
#include "point.h"
#include "menu.h"
#include "moveable.h"
#include "view.h"

#pragma once

enum variant_s : unsigned char {
	None, Attribute, Location, Role, Skill,
};
enum attribute_s : unsigned char {
	Strenght, Agility, Wits, Empathy
};
enum skill_s : unsigned char {
	Endure, Force, Fight, Sneak, Move, Shoot, Scout, Comprehend, KnowTheZone, SenceEmotion, Manipulate, Heal, Intimidate,
};
enum item_s : unsigned char {
	NoItem, Knife, Club, Sword, Battleaxe,
};
enum role_s : unsigned char {
	Enforcer, Gearhead, Stalker, Fixer, DogHandler, Chronicler, Boss, Slave,
};
typedef short unsigned	indext;
typedef dataset<attribute_s, Empathy> attributea;
typedef dataset<skill_s, Intimidate> skilla;
const indext			Blocked = 0xFFFF;
struct attributei {
	const char*			id;
	const char*			name;
};
struct itemi {
	const char*			id;
	const char*			name;
	const char*			text;
};
struct rolei {
	const char*			id;
	const char*			name;
	const char*			text;
};
class item {
	item_s				type;
	union {
		unsigned char	count;
		struct {
			int			quility : 3;
			bool		modified : 1;
		};
	};
public:
	constexpr item() : type(NoItem), count(0) {}
};
class statable {
	attributea			attributes;
	skilla				skills;
public:
	int					get(attribute_s i) const { return attributes.get(i); }
	int					get(skill_s i) const { return skills.get(i); }
	void				set(attribute_s i, int v) { attributes.set(i, v); }
	void				set(skill_s i, int v) { skills.set(i, v); }
};
class character : public statable {
};
class worldmap : public datamap<25 * 20> {
public:
	static constexpr int xmax = 25;
	static constexpr int ymax = 20;
	static indext		geti(int x, int y) { return y * xmax + x; }
	static point		getp(int x, int y);
	static int			getx(indext i) { return i % xmax; }
	static int			gety(indext i) { return i / xmax; }
	void				explore(indext i);
	void				fow();
	void				paint();
	void				set(indext i, int v);
	void				set(indext i, int r, int v, bool check);
};
struct locationi {
	static constexpr int kind = Location;
	const char*			id;
	posable				world;
	const char*			name;
	const char*			text;
	explicit constexpr operator bool() const { return name != 0; }
	int					getvelocity() const { return 50; }
	void				paint() const;
};
class gamei {
	static constexpr unsigned rounds_per_day = 24 * 4;
	worldmap			world;
	moveable			party;
	unsigned			rounds;
	static void			render_world();
	static void			update_moving();
	static void			worldmap_input();
public:
	static indext		geti(point v) { return worldmap::geti(v.x / gui.grid, v.y / gui.grid); }
	static point		getp(indext v);
	static void			playworld();
	static void			main_menu();
	void				setexplored(indext i) { setexplored(i, 1); }
	void				setexplored(indext i, int r);
	void				setmovement(point v) { party.setmovement(v, rounds); }
	void				setposition(point v) { party.setposition(v); }
};
extern gamei			game;