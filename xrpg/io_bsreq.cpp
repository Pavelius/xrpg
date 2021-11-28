#include "bsreq.h"
#include "log.h"
#include "stringbuilder.h"
#include "variant.h"

static char			temp[512];
static const char*	p;
static bool			allow_continue;

namespace {
struct valuei {
	const char*		text;
	long			number;
	void*			data;
	void clear() { memset(this, 0, sizeof(*this)); }
};
}

static void next() {
	while(true) {
		if(p[0] == '/' && p[1] == '*') {
			p += 2;
			while(*p) {
				if(p[0] == '*' && p[1] == '/') {
					p += 2;
					break;
				} else
					p++;
			}
		} else if(p[0] == '/' && p[1] == '/') {
			p += 2;
			while(*p) {
				if(*p == 10 || *p == 13)
					break;
				else
					p++;
			}
		} else if(*p == ' ' || *p == 9) {
			while(*p == ' ' || *p == 9)
				p++;
		} else
			break;
	}
}

static void skipsymcr() {
	if(!allow_continue)
		return;
	next();
	if(*p == 0)
		return;
	if(p[0] == 10 || p[0] == 13) {
		while(*p == 10 || *p == 13)
			p++;
		return;
	}
	log::error(p, "Expected line feed");
	allow_continue = false;
}

static void skipsym(char sym) {
	if(!allow_continue)
		return;
	if(*p == sym) {
		p = p + 1;
		next();
		return;
	}
	char temp[2] = {sym, 0};
	log::error(p, "Expected symbol `%1`", temp);
	allow_continue = false;
}

static const bsreq* getkey(const bsreq* type) {
	return type->find("id", bsmeta<const char*>::meta);
}

static void readid() {
	stringbuilder sb(temp); temp[0] = 0;
	if(!ischa(*p)) {
		log::error(p, "Expected identifier");
		allow_continue = false;
	} else
		p = sb.psidf(p);
	next();
}

static bool isvalue() {
	return (p[0] == '-' && isnum(p[1]))
		|| (p[0] == '\"')
		|| isnum(p[0])
		|| ischa(p[0]);
}

static void read_value(valuei& e, const bsreq* req) {
	e.clear();
	if(*p == '\"') {
		stringbuilder sb(temp);
		p = sb.psstr(p + 1, *p);
		e.text = szdup(temp);
	} else if(*p == '-' || isnum(*p)) {
		auto minus = false;
		if(*p == '-') {
			minus = true;
			p++;
		}
		p = stringbuilder::read(p, e.number);
		if(minus)
			e.number = -e.number;
	} else if(ischa(p[0])) {
		readid();
		if(!req) {
			// Nothing to do
		} else if(req->is(KindText))
			e.text = szdup(temp);
		else if(req->type == bsmeta<variant>::meta) {
			variant v1 = (const char*)temp;
			if(!v1)
				log::error(p, "Can't find variant `%1`", temp);
			e.number = v1.u;
		} else {
			auto offset = 0;
			auto pk = getkey(req->type);
			if(pk)
				offset = pk->offset;
			if(!req->source)
				log::error(p, "Invalid source array where read identifier `%1`", temp);
			else {
				e.number = req->source->find(temp, offset);
				if(e.number == -1) {
					log::error(p, "Not found identifier `%1`", temp);
					e.number = 0;
				} else
					e.data = req->source->ptr(e.number);
			}
		}
	}
	next();
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

static void write_value(void* object, const bsreq* req, int index, const valuei& v) {
	if(!req)
		return;
	auto p1 = req->ptr(object, index);
	if(req->is(KindSlice)) {
		auto ps = (sliceu<int>*)req->ptr(object);
		auto ci = req->source->getcount();
		req->source->add();
		if(!ps->count)
			ps->start = ci;
		p1 = (char*)req->source->ptr(ps->start + (ps->count++));
		req->set(p1, v.number);
	} else if(req->is(KindNumber) || req->is(KindEnum) || req->type == bsmeta<variant>::meta)
		req->set(p1, v.number);
	else if(req->is(KindText))
		req->set(p1, (long)szdup(v.text));
	else if(req->is(KindScalar))
		write_value(req->ptr(object), req->type + index, 0, v);
	else if(req->is(KindADat)) {
		auto p2 = (char*)req->ptr(object);
		auto pc = (int*)(p2 + FO(adat<char>, count));
		auto pd = p2 + FO(adat<char>, data);
		if(index > (int)req->count)
			index = 0;
		req->set(pd + index * req->size, v.number);
		if(index + 1 > *pc)
			*pc = index + 1;
	} else if(req->is(KindDSet))
		req->set(p1, v.number);
	else if(req->is(KindFlags)) {
		auto data = (unsigned char*)req->ptr(object);
		data[v.number / 8] |= 1 << (v.number % 8);
	} else if(req->is(KindReference))
		req->set(p1, (long)v.data);
	else
		log::error(p, "Unknown type in requisit `%1`", req->id);
}

static void fill(void* object, const bsreq* type, const valuei* keys, int key_count) {
	for(int i = 0; i < key_count; i++)
		write_value(object, type + i, 0, keys[i]);
}

static void* find_object(array* source, const bsreq* type, valuei* keys, int key_count) {
	auto pe = source->end();
	for(auto p = source->begin(); p < pe; p += source->size) {
		if(compare(p, type, keys, key_count))
			return p;
	}
	return 0;
}

static void read_dset(void* object, const bsreq* req) {
	auto index = 0;
	while(allow_continue && isvalue()) {
		valuei v;
		readid();
		index = req->source->find(temp, 0);
		if(index == -1) {
			index = 0;
			log::error(p, "Not found field `%1` in dataset `%2`", temp, req->id);
		}
		skipsym('(');
		read_value(v, req);
		skipsym(')');
		write_value(object, req, index++, v);
	}
}

static void read_array(void* object, const bsreq* req) {
	auto index = 0;
	while(allow_continue && isvalue()) {
		valuei v;
		read_value(v, req);
		write_value(object, req, index++, v);
	}
}

static bool islevel(int level) {
	auto push_p = p;
	p = skipsp(p);
	auto i = p - push_p;
	if(i == level && ischa(*p))
		return true;
	p = push_p;
	return false;
}

const bsreq* find_requisit(const bsreq* type, const char* id) {
	auto req = type->find(temp);
	if(!req)
		log::error(p, "Not found requisit `%1`", id);
	return req;
}

static void read_dictionary(void* object, const bsreq* type, int level, bool need_linefeed = true) {
	while(allow_continue && ischa(*p)) {
		readid();
		auto req = find_requisit(type, temp);
		skipsym('(');
		read_array(object, req);
		skipsym(')');
	}
	if(need_linefeed) {
		skipsymcr();
		while(allow_continue && islevel(level + 1)) {
			readid();
			auto req = type->find(temp);
			if(!req) {
				log::error(p, "Not found requisit `%1`", temp);
				allow_continue = false;
			} else if(req->is(KindDSet))
				read_dset(object, req);
			else if(req->is(KindScalar))
				read_dictionary(req->ptr(object), req->type, level + 1, false);
			else
				read_array(object, req);
			skipsymcr();
		}
	}
}

static void clear_object(void* object, const bsreq* type) {
	for(auto req = type; *req; req++) {
		auto p = req->ptr(object);
		memset(p, 0, req->lenght);
	}
}

static void* read_object(const bsreq* type, array* source, int key_count, int level) {
	if(!isvalue()) {
		log::error(p, "Expected value");
		allow_continue = false;
	}
	if(!key_count)
		key_count = 1;
	valuei keys[8] = {};
	for(auto i = 0; i < key_count; i++)
		read_value(keys[i], type + i);
	auto object = find_object(source, type, keys, key_count);
	if(!object) {
		object = source->add();
		clear_object(object, type);
		fill(object, type, keys, key_count);
	}
	read_dictionary(object, type + key_count, level);
	return object;
}

static varianti* find_type(const char* id) {
	for(auto& e : bsdata<varianti>()) {
		if(!e.source || !e.id)
			continue;
		if(strcmp(e.id, id) == 0)
			return &e;
	}
	return 0;
}

static void parse() {
	while(*p && allow_continue) {
		skipsym('#');
		readid();
		auto pd = find_type(temp);
		if(!pd) {
			if(temp[0])
				log::error(p, "Not find data type for `%1`", temp);
			return;
		}
		skipsymcr();
		while(allow_continue && isvalue())
			read_object(pd->metadata, pd->source, pd->key_count, 0);
	}
}

void bsreq::read(const char* url) {
	log::seturl(url);
	auto pb = loadt(url);
	if(!pb) {
		log::error(0, "Not found file `%1`", url);
		return;
	}
	log::setfile(pb);
	p = pb;
	allow_continue = true;
	parse();
	delete pb;
}