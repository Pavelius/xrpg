#include "bsreq.h"
#include "crt.h"
#include "flagable.h"
#include "io_plugin.h"
#include "log.h"
#include "variant.h"

using namespace io;

static const char* error_url;

BSMETA(flagable<1>) = {{}};
BSMETA(flagable<2>) = {{}};
BSMETA(flagable<4>) = {{}};

bool readf(const char* url) {
	struct proxy : serializer::reader {
		static varianti* findsource(const char* id) {
			for(auto& e : bsdata<varianti>()) {
				if(equal(e.id, id))
					return &e;
			}
			return 0;
		}
		static void* findobject(varianti* metadata, const char* id, bool create = true) {
			if(!metadata || !metadata->metadata)
				return 0;
			auto ps = metadata->source;
			if(!ps)
				return 0;
			id = szdup(id);
			auto ri = ps->find(id, 0);
			if(ri == -1) {
				if(create) {
					ri = ps->indexof(ps->addz());
					if(ri == -1)
						return 0;
					auto p = ps->ptr(ri);
					*((const char**)p) = id;
				} else
					return 0;
			}
			return ps->ptr(ri);
		}
		static const bsreq* getmeta(const serializer::node& e) {
			auto p = &e;
			while(!p->metadata && p->parent)
				p = p->parent;
			return (bsreq*)p->metadata;
		}
		static void* getobject(const serializer::node& e) {
			auto p = &e;
			while(!p->object && p->parent)
				p = p->parent;
			return p->object;
		}
		void warning(const char* text, ...) {
			if(error_url) {
				log::error("Error loading %1", error_url);
				error_url = 0;
			}
			char temp[4096]; stringbuilder sb(temp);
			sb.addv(text, xva_start(text));
			log::error(temp);
		}
		void open(serializer::node& e) override {
			switch(e.getlevel()) {
			case 0:
				if(e.name[0] != 0)
					e.skip = true;
				break;
			case 1:
				e.object = findsource(e.name);
				if(!e.object) {
					warning("Not found data type \'%1\'", e.name);
					e.skip = true;
				}
				break;
			case 2:
				e.metadata = (void*)((varianti*)e.parent->object)->metadata;
				if(e.parent->type == serializer::kind::Array)
					e.object = ((varianti*)e.parent->object)->source->addz();
				else
					e.object = findobject((varianti*)e.parent->object, e.name);
				break;
			default:
				break;
			}
		}
		void close(serializer::node& e) override {
		}
		void set(serializer::node& e, const char* value) override {
			auto pm = getmeta(e)->find(e.name);
			auto index = (e.parent->type == serializer::kind::Array) ? e.index : 0;
			if(!pm)
				warning("Can't find requisit \"%1\" to load value %2", e.name, value);
			else if(pm->type == bsmeta<variants>::meta) {
				variant v(value);
				if(!v)
					warning("Can't find variant \"%1\"", value);
				else {
					auto ps = (variants*)pm->ptr(getobject(e), 0);
					if(!ps->count) {
						bsdata<variant>::source.reserve(bsdata<variant>::source.getcount() + 1);
						ps->start = bsdata<variant>::source.getcount();
					}
					auto pv = ps->begin();
					pv[ps->count++] = v;
				}
			} else if(pm->type == bsmeta<flagable<1>>::meta
				|| pm->type == bsmeta<flagable<2>>::meta
				|| pm->type == bsmeta<flagable<4>>::meta) {
				variant v(value);
				if(!v)
					warning("Can't find variant \"%1\"", value);
				else {
					auto ps = (unsigned char*)pm->ptr(getobject(e), e.index);
					ps[v.value / 8] |= (1 << (v.value % 8));
				}
			} else if(pm->type == bsmeta<variant>::meta) {
				variant v(value);
				if(!v)
					warning("Can't find variant \"%1\"", value);
				else {
					auto ps = (variant*)pm->ptr(getobject(e), index);
					*ps = v;
				}
			} else if(pm->type == bsmeta<int>::meta) {
				auto ps = pm->ptr(getobject(e), index);
				pm->set(ps, stringbuilder::getnum(value));
			} else if(pm->type == bsmeta<const char*>::meta) {
				auto ps = pm->ptr(getobject(e), index);
				pm->set(ps, (long)szdup(value));
			}
		}
		proxy() {
		}
	};
	error_url = url;
	proxy reader_proxy;
	return io::read(url, reader_proxy) && log::geterrors() == 0;
}