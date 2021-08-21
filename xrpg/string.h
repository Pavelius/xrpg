#pragma once

class string {
	const char*			p;
	long				count;
public:
	constexpr string() : p(0), count(0) {}
	constexpr string(const char* p) : p(p), count(end(p) - p) {}
	constexpr string(const char* p, long count) : p(p), count(count) {}
	constexpr const char* begin() const { return p; }
	constexpr const char* end() const { return p + count; }
	constexpr bool operator==(const char* id) const { for(auto i=0; i<count; i++) if(p[i] != id[i]) return false; return id[count]==0; }
	constexpr bool operator==(const string& e) const { return e.p == p && e.count == count; }
	constexpr bool operator!=(const string& e) const { return e.p != p || e.count != count; }
	static constexpr const char* end(const char* p) { if(!p) return 0; while(*p) p++; return p; }
	const char*			get() const;
	void				set(const char* p, unsigned v) { this->p = p; this->count = v; }
};
