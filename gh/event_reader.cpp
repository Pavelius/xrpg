#include "crt.h"
#include "io_stream.h"
#include "log.h"
#include "stringbuilder.h"
#include "variant.h"

struct dialogi {
	short				id, answer;
	const char*			text;
	variants			actions;
};

static bool isheader(const char* p) {
	return p[0] == '#' && isnum(p[1]);
}

static bool isanswer(const char* p) {
	return p[0] == '~' && isnum(p[1]);
}

static bool isresolve(const char* p) {
	return p[0] == '-' && p[1] == '-' && isnum(p[2]);
}

static const char* read_string_v1(const char* p, char* ps, const char* pe) {
	char sym;
	while(*p && !(isanswer(p) || isheader(p) || isresolve(p))) {
		sym = *p++;
		switch(sym) {
		case 17: sym = '-'; break;
		}
		if(ps < pe)
			*ps++ = sym;
	}
	*ps = 0;
	while(*p == '\n' || *p == '\r') {
		p = skipcr(p);
		p = skipsp(p);
	}
	return p;
}

static const char* read_block(const char* p, int& v) {
	char temp[8192]; v = 0;
	if(isnum(p[1])) {
		p = stringbuilder::read(p, v);
		p = skipspcr(p);
		p = read_string_v1(p, temp, temp + sizeof(temp) - 1);
	}
	return p;
}

static bool readevents(const char* url, array& source) {
	auto p_alloc = (const char*)loadt(url);
	if(!p_alloc)
		return false;
	auto p = p_alloc;
	char value[8192];
	auto stage = 0, answer = 0;
	while(*p) {
		if(p[0] != '#') {
			stage = answer = 0;
			p = read_block(p, stage);
		}
	}
	delete p_alloc;
	return true;
}