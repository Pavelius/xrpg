#pragma once

class string {
	const char*			p;
	unsigned			count;
public:
	constexpr string() : p(0), count(0) {}
	constexpr string(const char* p) : p(p), count(end(p) - p) {}
	constexpr string(const char* p, unsigned count) : p(p), count(count) {}
	constexpr const char* begin() const { return p; }
	constexpr const char* end() const { return p + count; }
	constexpr bool operator==(const char* id) const { for(unsigned i=0; i<count; i++) if(p[i] != id[i]) return false; return true; }
	constexpr bool operator==(const string& e) const { return e.p == p && e.count == count; }
	static constexpr const char* end(const char* p) { if(!p) return 0; while(*p) p++; return p; }
};
