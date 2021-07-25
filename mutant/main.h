#include "answers.h"
#include "datamap.h"
#include "point.h"
#include "menu.h"
#include "moveable.h"
#include "view.h"

#pragma once

enum variant_s : unsigned char {
	None, Attribute, Location, Skill,
};
enum attribute_s : unsigned char {
	Strenght, Agility, Wits, Empathy
};
enum skill_s : unsigned char {
	Endure, Force, Fight, Sneak, Move, Shoot, Scout, Comprehend, KnowTheZone, SenceEmotion, Manipulate, Heal, Intimidate,
};
typedef short unsigned	indext;
const indext			Blocked = 0xFFFF;
struct attributei {
	const char*			id;
	const char*			name;
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
struct locationi : public posable {
	static constexpr int kind = Location;
	const char*			id;
	const char*			name;
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