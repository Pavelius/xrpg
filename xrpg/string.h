#pragma once

class string {
	const char*			p;
public:
	long				size;
	constexpr string() : p(0), size(0) {}
	constexpr string(const char* p) : p(p), size(end(p) - p) {}
	constexpr string(const char* p, long size) : p(p), size(size) {}
	constexpr const char* begin() const { return p; }
	constexpr const char* end() const { return p + size; }
	constexpr bool operator==(const char* id) const { for(auto i=0; i< size; i++) if(p[i] != id[i]) return false; return id[size]==0; }
	constexpr bool operator==(const string& e) const { return e.p == p && e.size == size; }
	constexpr bool operator!=(const string& e) const { return e.p != p || e.size != size; }
	static constexpr const char* end(const char* p) { if(!p) return 0; while(*p) p++; return p; }
	bool				equal(const string& v) const;
	const char*			get() const;
	bool				has(const char* p) const { return p >= begin() && p <= end(); }
	void				set(const char* p, unsigned v) { this->p = p; this->size = v; }
};
