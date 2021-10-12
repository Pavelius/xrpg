#include "bsreq.h"
#include "log.h"
#include "stringbuilder.h"
#include "variant.h"

static char			temp[512];

struct valuei {
	const char*		text;
	long			number;
	void*			data;
	void clear() { memset(this, 0, sizeof(*this)); }
};

static const char* skipsym(const char* p, char sym) {
    if(sym==10) {
        if(*p==10 || *p==13)
            return skipcr(p);
        log::errorv(p, "Expected line feed");
    } else {
        if(*p==sym)
            return p + 1;
        char temp[2] = {sym, 0};
        log::error(p, "Expected symbol `%1`", temp);
    }
    return "";
}

static const bsreq* getkey(const bsreq* type) {
	return type->find("id", bsmeta<const char*>::meta);
}

static const char* readid(const char* p) {
	stringbuilder sb(temp); temp[0] = 0;
	if(!ischa(*p)) {
		log::error(p, "Expected identifier");
		return p;
	}
	p = sb.psidf(p);
	return skipsp(p);
}

static const char* readv(const char* p, const bsreq* req, valuei& e) {
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
	} else {
		p = readid(p);
		if(p[0] == '(')
			log::error(p, "Expected idetifier");
		else if(req->is(KindText))
			e.text = szdup(temp);
		else if(req->type==bsmeta<variant>::meta) {
            variant v1 = (const char*)temp;
            e.number = v1.u;
		} else {
            auto pk = getkey(req->type);
            if(!pk)
                log::error(p, "Requisit don't have source type when load identifier `%1`", temp);
            else if(!req->source)
                log::error(p, "Invalid source array where read identifier `%1`", temp);
            else {
                e.number = req->source->find(temp, pk->offset);
                if(e.number == -1) {
                    log::error(p, "Not found identifier `%1`", temp);
                    e.number = 0;
                } else
                    e.data = req->source->ptr(e.number);
            }
		}
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

static void writev(void* p, const bsreq* req, int index, const valuei& v) {
    auto p1 = req->ptr(p, index);
    if(req->is(KindNumber) || req->type==bsmeta<variant>::meta)
        req->set(p1, v.number);
    else if(req->is(KindText))
        req->set(p1, (long)szdup(v.text));
    else if(req->is(KindReference))
        req->set(p1, (long)v.data);
}

static void fill(void* p, const bsreq* type, const valuei* keys, int key_count) {
	for(int i = 0; i < key_count; i++)
        writev(p, type + i, 0, keys[i]);
}

static void* findv(array* source, const bsreq* type, valuei* keys, int key_count) {
	auto pe = source->end();
	for(auto p = source->begin(); p < pe; p += source->size) {
		if(compare(p, type, keys, key_count))
			return p;
	}
	return 0;
}

static const char* read_array(const char* p, void* object, const bsreq* req) {
    auto index = 0;
    while(*p && *p!=')') {
        valuei v;
        p = readv(p, req, v);
        writev(object, req, index++, v);
    }
    if(*p==')')
        p = skipsp(p+1);
	return p;
}

static const char* read_dictionary(const char* p, void* object, const bsreq* type) {
    while(*p && !(*p==13 || *p==10)) {
        p = readid(p);
        auto req = type->find(temp);
        if(!req)
            log::error(p, "Not found requisit `%1`", temp);
        p = skipsym(p, '(');
        p = read_array(p, object, type);
    }
	return p;
}

static const char* readv(const char* p, const bsreq* type, array* source, int key_count, void** result_object) {
	if(!key_count)
        key_count = 1;
	valuei keys[8] = {};
	for(auto i = 0; i < key_count; i++)
		p = readv(p, type + i, keys[i]);
	auto object = findv(source, type, keys, key_count);
	if(!object) {
        object = source->add();
        fill(object, type, keys, key_count);
	}
	return read_dictionary(p, object, type);
}

static varianti* findbase(const char* id) {
    for(auto& e : bsdata<varianti>()) {
        if(!e.source || !e.id)
            continue;
        if(strcmp(e.id, id)==0)
            return &e;
    }
	return 0;
}

static void parse(const char* p) {
	while(*p) {
        p = skipsp(skipsym(p, '#'));
		p = readid(p + 1);
		auto pd = findbase(temp);
		if(!pd && temp[0]) {
			log::error(p, "Not find data type for `%1`", temp);
			return;
		}
        p = skipsp(skipsym(p, 10));
		while(*p) {
            void* object = 0;
            p = readv(p, pd->metadata, pd->source, pd->key_count, &object);
            p = skipspcr(p);
		}
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
    parse(pb);
    delete pb;
}