#pragma once

template<class T, unsigned N, class DT = char>
class dataset {
	DT				data[N + 1];
public:
	constexpr dataset() : data() {}
	void			add(T i) { data[i] += 1; }
	void			add(T i, int v) { data[i] += v; }
	const DT*		begin() const { return data; }
	void			clear() { for(auto& e : data) e = 0; }
	const DT*		end() const { return data + sizeof(data)/sizeof(data[0]); }
	constexpr int	get(T i) const { return data[i]; }
	constexpr void	set(T i, int v) { data[i] = v; }
};
