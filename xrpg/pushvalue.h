#pragma once

template<typename T>
class pushvalue {
	T& value;
	T stored;
public:
	constexpr pushvalue(T& value, T new_value) : value(value), stored(value) { value = new_value; }
	~pushvalue() { value = stored; }
};
