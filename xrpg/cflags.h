#include <initializer_list>

#pragma once

// Abstract flag data bazed on enumerator
template<typename T, typename DT = unsigned>
class cflags {
	DT								data;
public:
	constexpr cflags() : data(0) {}
	constexpr cflags(const std::initializer_list<T> list) : data() { for(auto e : list) add(e); }
	constexpr void					add(const T id) { data |= 1 << id; }
	constexpr void					clear() { data = 0; }
	constexpr bool					is(const T id) const { return (data & (1 << id)) != 0; }
	constexpr void					remove(T id) { data &= ~(1 << id); }
};
