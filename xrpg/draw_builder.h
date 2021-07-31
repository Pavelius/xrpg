#pragma once

typedef bool (*fnallowid)(const void* object, const char* id);
typedef const char** (*fngetcommands)(const void* object, const char* id);

namespace draw {
class builder {
	bool					separator;
	int						count;
	void					render(const char** commands);
public:
	const void*				object;
	fnallowid				allowid;
	fngetcommands			getcommands;
	constexpr builder() : count(), separator(), object(), allowid(), getcommands() {}
	virtual ~builder() {}
	virtual void			add(const char* id) = 0;
	virtual void			addseparator() = 0;
	virtual const char*		finish() const { return 0; }
	void					render(const char** commands, const void* object, fnallowid proc = 0, fngetcommands getcommands = 0);
	virtual void			start() const {}
};
}
