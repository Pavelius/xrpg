#include "crt.h"
#include "io.h"

#pragma once

// Fast and simple driver for streaming binary data
struct archive {
	io::stream&			source;
	bool				writemode;
	constexpr archive(io::stream& source, bool writemode) : source(source), writemode(writemode) {}
	bool				signature(const char* id);
	bool				version(short major, short minor);
	// Array with fixed count
	template<typename T, unsigned N> void set(T(&value)[N]) {
		for(int i = 0; i < N; i++)
			set(value[i]);
	};
	// Fixed data collection
	template<typename T, unsigned N> void set(adat<T, N>& value) {
		set(value.count);
		for(auto& e : value)
			set(e);
	}
	// All simple types and requisites
	template<class T> void set(T& value) {
		set(&value, sizeof(value));
	}
	void set(array& value) {
		set(value.count);
		set(value.size);
		if(!writemode)
			value.reserve(value.count);
		set(value.data, value.size * value.count);
	}
	void set(void* value, unsigned size) {
		if(writemode)
			source.write(value, size);
		else
			source.read(value, size);
	}
};