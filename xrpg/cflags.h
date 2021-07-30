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
// Abstract flag set
template<unsigned last>
class flagable {
	static constexpr unsigned s = 8;
	static constexpr unsigned c = 1 + last / s;
	unsigned char					data[c];
public:
	constexpr flagable() : data{0} {}
	template<class T> constexpr flagable(const std::initializer_list<T> v) : data{0} { for(auto e : v) set(e); }
	constexpr void					add(const flagable& e) { for(unsigned i = 0; i < c; i++) data[i] |= e.data[i]; }
	void							clear() { memset(this, 0, sizeof(*this)); }
	constexpr bool					is(short unsigned v) const { return (data[v / s] & (1 << (v % s))) != 0; }
	constexpr bool					is(const flagable& e) const { for(unsigned i = 0; i < c; i++) if((data[i] & e.data[i]) != 0) return true; return false; }
	constexpr void					remove(short unsigned v) { data[v / s] &= ~(1 << (v % s)); }
	constexpr void					set(short unsigned v) { data[v / s] |= 1 << (v % s); }
	constexpr void					set(short unsigned v, bool activate) { if(activate) set(v); else remove(v); }
};