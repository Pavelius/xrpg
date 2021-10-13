#include "bsreq.h"
#include "log.h"
#include "stringbuilder.h"
#include "variant.h"

static char			temp[512];
static const char*	p;

struct valuei {
	const char*		text;
	long			number;
	void*			data;
	void clear() { memset(this, 0, sizeof(*this)); }
};

static void next() {
    while(*p == ' ' || *p == 9)
        p++;
}

static void nextline() {
    while(*p == 10 || *p == 13)
        p++;
}

static bool iscr() {
	return p[0] == 10 || p[0] == 13;
}

static void skipsym(char sym) {
    if(sym==10) {
		if(iscr()) {
			while(iscr())
				p = skipcr(p);
			return;
		}
        log::error(p, "Expected line feed");
    } else {
		if(*p == sym) {
			p = p + 1;
			next();
			return;
		}
        char temp[2] = {sym, 0};
        log::error(p, "Expected symbol `%1`", temp);
    }
}

static const bsreq* getkey(const bsreq* type) {
	return type->find("id", bsmeta<const char*>::meta);
}

static void readid() {
	stringbuilder sb(temp); temp[0] = 0;
	if(!ischa(*p))
		log::error(p, "Expected identifier");
	else
		p = sb.psidf(p);
	next();
}

static bool isvalue() {
    return (p[0]=='-' && isnum(p[1]))
        || (p[0]=='\"')
        || isnum(p[0])
        || ischa(p[0]);
}

static void read_value(valuei& e, const bsreq* req) {
	e.clear();
	if(*p == '\"') {
		stringbuilder sb(temp);
		p = sb.psstr(p + 1, *p);
		e.text = szdup(temp);
        next();
	} else if(*p == '-' || isnum(*p)) {
		auto minus = false;
		if(*p == '-') {
			minus = true;
			p++;
		}
		p = stringbuilder::read(p, e.number);
		if(minus)
			e.number = -e.number;
        next();
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
            auto pk = getkey(req->type);
            if(!pk)
                log::error(p, "Requisit don't have key when load identifier `%1`", temp);
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
    if(req->is(KindNumber) || req->type==bsmeta<variant>::meta)
        req->set(p1, v.number);
    else if(req->is(KindText))
        req->set(p1, (long)szdup(v.text));
    else if(req->is(KindReference))
        req->set(p1, (long)v.data);
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
    while(*p && isvalue()) {
        valuei v;
        readid();
        index = req->source->find(temp, 0);
        if(index==-1) {
            index = 0;
            log::error(p, "Not found field `%1` in array of requisit `%2`", temp, req->id);
        }
        skipsym('(');
        read_value(v, req);
        skipsym(')');
        write_value(object, req, index++, v);
		next();
    }
}

static void read_array(void* object, const bsreq* req) {
    auto index = 0;
    while(*p && isvalue()) {
        valuei v;
        read_value(v, req);
        write_value(object, req, index++, v);
    }
}

static bool islevel(int level) {
	if(!iscr())
		return false;
	auto push_p = p;
	p = skipcr(p);
	auto pb = p;
	p = skipsp(p);
	auto i = p - pb;
	if(i == level && ischa(*p))
		return true;
	p = push_p;
	return false;
}

static void read_dictionary(void* object, const bsreq* type, int level) {
    while(ischa(*p)) {
        readid();
        auto req = type->find(temp);
        if(!req)
            log::error(p, "Not found requisit `%1`", temp);
        skipsym('(');
        read_array(object, req);
		skipsym(')');
    }
    auto pb = p;
	while(islevel(level + 1)) {
		readid();
		auto req = type->find(temp);
		if(!req)
			log::error(p, "Not found requisit `%1`", temp);
        else if(req->is(KindDSet))
            read_dset(object, req);
		else if(req->is(KindScalar))
			read_dictionary(req->ptr(object), req->type, level + 1);
		else
			read_array(object, type);
        if(pb==p)
            break;
        pb = p;
	}
}

static void* read_object(const bsreq* type, array* source, int key_count, int level) {
	if(!key_count)
        key_count = 1;
	valuei keys[8] = {};
	for(auto i = 0; i < key_count; i++)
		read_value(keys[i], type + i);
	auto object = find_object(source, type, keys, key_count);
	if(!object) {
        object = source->add();
        fill(object, type, keys, key_count);
	}
	read_dictionary(object, type, level);
	return object;
}

static varianti* find_type(const char* id) {
    for(auto& e : bsdata<varianti>()) {
        if(!e.source || !e.id)
            continue;
        if(strcmp(e.id, id)==0)
            return &e;
    }
	return 0;
}

static void parse() {
    auto pb = p;
	while(*p) {
		skipsym('#');
		readid();
		auto pd = find_type(temp);
		if(!pd) {
			if(temp[0])
			log::error(p, "Not find data type for `%1`", temp);
			return;
		}
		skipsym(10);
		while(*p && *p!='#') {
            read_object(pd->metadata, pd->source, pd->key_count, 0);
			nextline();
		}
		if(pb == p)
			break;
        pb = p;
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
    parse();
    delete pb;
}