#include "bsreq.h"
#include "crt.h"
#include "io_plugin.h"

using namespace io;

bool bsreq::custom::readf(const char* url) {
	struct proxy : serializer::reader {
		bsreq::custom& custom;
		const bsreq* findsource(const char* id) const {
			for(auto pe = custom.source; *pe; pe++) {
				if(strcmp(pe->id, id) == 0)
					return pe;
			}
			return 0;
		}
		void* findobject(bsreq* metadata, const char* id, bool create) {
			if(!metadata || !metadata->type)
				return 0;
			auto ps = metadata->source;
			if(!ps)
				return 0;
			id = szdup(id);
			auto ri = ps->find(id, 0);
			if(ri == -1) {
				if(create) {
					ri = ps->indexof(metadata->source->add());
					if(ri == -1)
						return 0;
					auto p = ps->ptr(ri);
					*((const char**)p) = id;
					custom.create(metadata->type, p);
				} else
					return 0;
			}
			return ps->ptr(ri);
		}
		static const bsreq* getmeta(const serializer::node& e) {
			return (bsreq*)e.metadata;
		}
		static void* getobject(const serializer::node& e) {
			return e.object;
		}
		void open(serializer::node& e) override {
			switch(e.getlevel()) {
			case 0:
				if(e.name[0] != 0)
					e.skip = true;
				break;
			case 1:
				e.object = (void*)findsource(e.name);
				break;
			case 2:
				e.metadata = (void*)((bsreq*)e.parent->object)->type;
				e.object = findobject((bsreq*)e.parent->object, e.name, true);
				break;
			default:
				e.metadata = (void*)getmeta(*e.parent)->find(e.name);
				if(e.metadata) {
					e.object = getmeta(e)->ptr(e.parent->object);
					e.metadata = (void*)((bsreq*)e.metadata)->type;
				} else
					e.object = 0;
				break;
			}
		}
		static int getnumber(const char* v) {
			int result; stringbuilder::read(v, result);
			return result;
		}
		void set(serializer::node& e, const char* value) override {
			auto pm = getmeta(*e.parent)->find(e.name);
			if(!pm && e.name == "element")
				pm = getmeta(*e.parent);
			else
				e.index = 0;
			if(!pm) {
				error("Can't find requisit \"%1\"", e.name);
				return;
			}
			auto ps = pm->ptr(getobject(*e.parent), e.index);
			if(pm->type == bsmeta<int>::meta)
				pm->set(ps, getnumber(value));
			else if(pm->type == bsmeta<const char*>::meta)
				pm->set(ps, (long)szdup(value));
			else if(pm->is(KindEnum)) {
				auto i = pm->findenum(value);
				if(i == -1) {
					error("Can't find enum value \"%1\"", value);
					return;
				}
				pm->set(ps, i);
			} else if(pm->is(KindCFlags)) {
				auto i = pm->findenum(value);
				if(i==-1) {
					error("Can't find enum value \"%1\"", value);
					return;
				}
				auto n = pm->get(ps);
				n |= 1 << i;
				pm->set(ps, n);
			} else
				custom.read(this, pm, e, value);
		}
		proxy(bsreq::custom& custom) : custom(custom) {
		}
	};
	proxy reader_proxy(*this);
	if(!reader_proxy.custom.source) {
		reader_proxy.error("Requisit \'sources\' can't be empthy");
		return false;
	}
	return io::read(url, reader_proxy);
}