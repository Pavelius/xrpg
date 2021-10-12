#include "bsreq.h"
#include "log.h"
#include "stringbuilder.h"

static char			temp[512];

struct valuei {
	const char*		text;
	long			number;
	void*			data;
	void clear() { memset(this, 0, sizeof(*this)); }
};

static const bsreq* getkey(const bsreq* type) {
	return type->find("id", bsmeta<const char*>::meta);
}

static void parseid(const char* p, valuei& e, const bsreq* type, const array* source) {
	auto pk = getkey(type);
	if(!type)
		log::error(p, "Value type not have key to find identifier `%1`", temp);
	else {
		e.number = source->find(temp, pk->offset);
		if(e.number == -1) {
			log::error(p, "Not found identifier `%1`", temp);
			e.number = 0;
		} else
			e.data = source->ptr(e.number);
	}
}

static const char* readv(const char* p, valuei& e, const bsreq* type, const array* source);

static const char* readid(const char* p) {
	if(!ischa(*p)) {
		log::error(p, "Expected identifier");
		valuei e = {};
		readv(p, e, 0, 0);
		return p;
	}
	stringbuilder sb(temp);
	p = sb.psidf(p + 1);
	return skipsp(p);
}

static const char* readv(const char* p, valuei& e, const bsreq* type, const array* source) {
	e.clear();
	if(*p == '\"') {
		stringbuilder sb(temp);
		p = sb.psstr(p + 1, *p);
		e.text = szdup(temp);
	} else if(*p == '-' || isnum(*p)) {
		auto minus = false;
		if(*p == '-')
			minus = true;
		p = stringbuilder::read(p + 1, e.number);
		if(minus)
			e.number = -e.number;
	} else {
		p = readid(p);
		if(p[0] == '(')
			log::error(p, "Expected idetifier");
		else if(type->is(KindText))
			e.text = szdup(temp);
		else if(!source)
			log::error(p, "Invalid identifier `%1` to read in value", temp);
		else
			parseid(p, e, type, source);
	}
	return skipsp(p);
}

static bool compare(const void* p, const bsreq* requisit, const valuei& value) {
	auto pv = requisit->ptr(p);
	if(requisit->is(KindText)) {
		auto v1 = *((const char**)pv);
		auto v2 = value.text;
		if(!v1 && !v2)
			return true;
		if(v1 && v2)
			return (strcmp(v1, v2) == 0);
	} else {
		auto v1 = requisit->get(pv);
		return value.number == v1;
	}
	return false;
}

static bool compare(const void* p, const bsreq* type, const valuei* keys, int key_count) {
	for(int i = 0; i < key_count; i++) {
		if(!compare(p, type + i, keys[i]))
			return false;
	}
	return true;
}

static void* findv(array* source, const bsreq* type, valuei* keys, int key_count) {
	auto pe = source->end();
	for(auto p = source->begin(); p < pe; p += source->size) {
		if(compare(p, type, keys, key_count))
			return p;
	}
	return 0;
}

static const char* read(const char* p, const bsreq* type, array* source, int key_count) {
	valuei keys[8] = {};
	for(auto i = 0; i < key_count; i++)
		p = readv(p, keys[i], type[i].type, type[i].source);
	auto object = findv(source, type, keys, key_count);
	return p;
}

static array* findbase(const char* id) {
	return 0;
}

static void parse(const char* p) {
	while(*p) {
		if(*p != '#') {
			log::error(p, "Expected symbol `#`");
			return;
		}
		p = readid(p + 1);
		auto base = findbase(temp);
		while(*p) {

		}
	}
}