#include "answers.h"
#include "point.h"
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
template<int count>
class datamap {
	constexpr static unsigned perbyte = 4;
	unsigned char		data[(count + perbyte - 1) / perbyte];
public:
	int					get(short unsigned i) const { return (data[i / perbyte] >> ((i % perbyte) * 2)) & 0x03; }
	void				set(short unsigned i, int v) { data[i / perbyte] |= (v & 0x03) << ((i % perbyte) * 2); }
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
struct locationi : public moveable {
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
	static void			click_order();
public:
	static indext		geti(point v) { return worldmap::geti(v.x / gui.grid, v.y / gui.grid); }
	static void			playworld() { draw::scene(render_world, update_moving, click_order); }
	void				setexplored(indext i) { setexplored(i, 1); }
	void				setexplored(indext i, int r);
	void				setmovement(point v) { party.setmovement(v, rounds); }
	void				setposition(point v) { party.setposition(v); }
};
extern gamei			game;