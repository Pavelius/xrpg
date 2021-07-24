#include "posable.h"

#pragma once

class moveable : public posable {
	point			start_position, target_position;
	unsigned		start_tick;
public:
	bool			ismoving() const;
	bool			moving(int pixels_per_unit, unsigned tick, unsigned ticks_per_unit);
	point			getdestination() const { return target_position; }
	void			setposition(point v);
	void			setmovement(point v, unsigned tick);
	void			stop();
};