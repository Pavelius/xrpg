#include "moveable.h"

long distance(point p1, point p2);

bool moveable::ismoving() const {
	return position != target_position;
}

void moveable::stop() {
	setposition(position);
}

void moveable::setposition(point v) {
	position = v;
	start_position = v;
	target_position = v;
	start_tick = 0;
}

void moveable::setmovement(point v, unsigned tick) {
	start_position = position;
	target_position = v;
	start_tick = tick;
}

bool moveable::moving(int pixels_per_part, unsigned tick, unsigned ticks_per_part) {
	if(!ismoving())
		return false;
	long maximum = distance(start_position, target_position);
	if(!maximum) {
		position = target_position;
		return false;
	}
	long passed_time = tick - start_tick;
	long current = pixels_per_part * passed_time / ticks_per_part;
	if(current >= maximum)
		position = target_position;
	else {
		position.x = (short)(start_position.x + (target_position.x - start_position.x) * current / maximum);
		position.y = (short)(start_position.y + (target_position.y - start_position.y) * current / maximum);
	}
	return true;
}