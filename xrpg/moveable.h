#include "point.h"

#pragma once

class moveable {
	point			position;
	point			start_position, target_position;
	unsigned		start_tick;
public:
	bool			ismoving() const;
	bool			moving(int pixels_per_unit, unsigned tick, unsigned ticks_per_unit);
	point			getposition() const { return position; }
	void			setposition(point v);
	void			setmovement(point v, unsigned tick);
	void			stop();
	void			wait();
};