#pragma once

namespace draw {
typedef bool (*fnallowid)(const void* object, const char* id);
typedef const char** (*fngetcommands)(const void* object, const char* id);
class builder {
	bool					separator;
	int						count;
	void					render(const char** commands);
protected:
	const void*				object;
	fnallowid				allowid;
	fngetcommands			getcommands;
public:
	constexpr builder() : separator(), count(), object(), allowid(), getcommands() {}
	virtual ~builder() {}
	virtual void			add(const char* id) = 0;
	virtual void			addseparator() = 0;
	const char*				choose(const char** source, const void* object, fnallowid allowid = 0, fngetcommands getcommands = 0);
	virtual const char*		finish() const { return 0; }
	void					render(const char** commands, const void* object, fnallowid proc = 0, fngetcommands getcommands = 0);
	virtual void			start() const {}
};
}
