#include "point.h"

#pragma once

class posable {
	point			position;
public:
	point			getposition() const { return position; }
	void			setposition(point v) { position = v; }
};
