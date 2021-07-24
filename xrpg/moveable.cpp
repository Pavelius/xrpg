#include "moveable.h"

long distance(point p1, point p2);

bool moveable::ismoving() const {
	return getposition() != target_position;
}

void moveable::stop() {
	setposition(getposition());
}

void moveable::setposition(point v) {
	posable::setposition(v);
	start_position = v;
	target_position = v;
	start_tick = 0;
}

void moveable::setmovement(point v, unsigned tick) {
	start_position = getposition();
	target_position = v;
	start_tick = tick;
}

bool moveable::moving(int pixels_per_part, unsigned tick, unsigned ticks_per_part) {
	if(!ismoving())
		return false;
	long maximum = distance(start_position, target_position);
	if(!maximum) {
		setposition(target_position);
		return false;
	}
	long passed_time = tick - start_tick; 
	long current = pixels_per_part * passed_time / ticks_per_part;
	if(current >= maximum)
		setposition(target_position);
	else {
		auto x = (short)(start_position.x + (target_position.x - start_position.x) * current / maximum);
		auto y = (short)(start_position.y + (target_position.y - start_position.y) * current / maximum);
		posable::setposition({x, y});
	}
	return true;
}