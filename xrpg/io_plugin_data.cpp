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
		void warning(const char* text, ...) {
			if(error_url) {
				log::error("Error loading %1", error_url);
				error_url = 0;
			}
			char temp[4096]; stringbuilder sb(temp);
			sb.addv(text, xva_start(text));
			log::error(temp);
		}
		const bsreq* getmetadata(serializer::node& e) const {
			if(e.parent->type == serializer::kind::Array)
				return (bsreq*)e.parent->parent->metadata;
			else
				return (bsreq*)e.parent->metadata;
		}
		const char* getname(const serializer::node& e) const {
			if(e.parent->type == serializer::kind::Array)
				return e.parent->name;
			else
				return e.name;
		}
		const bsreq* getrequisit(serializer::node& e) const {
			auto type = getmetadata(e);
			auto name = getname(e);
			return type->find(name);
		}
		void testslice(const bsreq* req, serializer::node& e) {
			if(req->is(KindSlice)) {
				auto ps = (sliceu<int>*)e.object;
				e.object = req->source->addz();
				if(!ps->count)
					ps->start = req->source->indexof(e.object);
				ps->count++;
			}
		}
		void applyvalues(serializer::node& e) {
			const bsreq* req = 0;
			if(e.parent->type == serializer::kind::Struct)
				req = ((bsreq*)e.parent->metadata)->find(e.name);
			else
				req = (bsreq*)e.parent->requisit;
			if(!req) {
				e.skip = true;
				warning("Can't find requisit \"%1\"", e.name);
				return;
			}
			e.requisit = (void*)req;
			e.metadata = (void*)req->type;
			if(e.parent->type == serializer::kind::Struct)
				e.object = req->ptr(e.parent->object);
			else if(e.parent->type == serializer::kind::Array) {
				auto i = e.index;
				if(i >= (int)req->count)
					i = req->count - 1;
				if(req->is(KindSlice)) {
					auto ps = (sliceu<int>*)e.parent->object;
					e.object = req->source->addz();
					if(!ps->count)
						ps->start = req->source->indexof(e.object);
					ps->count++;
				} else
					e.object = (unsigned char*)e.parent->object + i * req->size;
			}
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
					warning("Not found type \'%1\'", e.name);
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
				applyvalues(e);
				break;
			}
		}
		void close(serializer::node& e) override {
			auto req = (const bsreq*)e.requisit;
			if(!req)
				return;
			if(req->is(KindSlice)) {
				auto ps = (sliceu<int>*)e.object;
				if(ps->count) {
					// TODO: make unique values
				}
			}
		}
		bool read(variant& v, const char* value) {
			v = value;
			if(!v) {
				warning("Can't find variant `%1`", value);
				return false;
			}
			return true;
		}
		void set(serializer::node& e, const char* value) override {
			if(e.name[0] == '$')
				return;
			applyvalues(e);
			auto req = (const bsreq*)e.requisit;
			if(!req)
				return;
			if(req->type == bsmeta<flagable<1>>::meta
				|| req->type == bsmeta<flagable<2>>::meta
				|| req->type == bsmeta<flagable<4>>::meta) {
				variant v;
				if(read(v, value)) {
					auto ps = (unsigned char*)e.object;
					ps[v.value / 8] |= (1 << (v.value % 8));
				}
			} else if(req->type == bsmeta<variant>::meta) {
				variant v;
				if(read(v, value)) {
					auto ps = (variant*)e.object;
					*ps = v;
				}
			} else if(req->type == bsmeta<int>::meta) {
				auto numeric = stringbuilder::getnum(value);
				req->set(e.object, numeric);
			} else if(req->type == bsmeta<const char*>::meta)
				req->set(e.object, (long)szdup(value));
		}
		proxy() {
		}
	};
	error_url = url;
	proxy reader_proxy;
	return io::read(url, reader_proxy) && log::geterrors() == 0;
}