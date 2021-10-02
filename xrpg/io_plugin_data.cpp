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
		const bsreq* getrequisit(serializer::node& e) {
			const bsreq* req = 0;
			switch(e.parent->type) {
			case serializer::kind::Struct:
				req = ((bsreq*)e.parent->metadata)->find(e.name);
				break;
			default:
				req = (bsreq*)e.parent->requisit;
				break;
			}
			if(!req) {
				e.skip = true;
				warning("Can't find requisit \"%1\"", e.name);
			}
			return req;
		}
		void applyvalues(serializer::node& e) {
			auto req = getrequisit(e);
			if(!req)
				return;
			e.requisit = (void*)req;
			e.metadata = (void*)req->type;
			if(e.parent->type == serializer::kind::Struct)
				e.object = req->ptr(e.parent->object);
			else if(e.parent->type == serializer::kind::Array) {
				auto i = e.index;
				if(i >= req->count)
					i = req->count - 1;
				if(req->is(KindSlice)) {
					auto ps = (sliceu<int>*)e.object;
					e.object = req->source->addz();
					if(!ps->count)
						ps->start = req->source->indexof(e.object);
					ps->count++;
				} else
					e.object = (unsigned char*)e.parent->object + i * req->size;
			}
		}
		const bsreq* readcontext(serializer::node& e, const char* value) {
			auto req = (const bsreq*)e.parent->metadata;
			if(!req) {
				warning("Parent metadata is null", e.name);
				e.skip = true;
				return 0;
			}
			if(e.parent->type == serializer::kind::Array) {
				req = (const bsreq*)e.parent->parent->metadata;
				req = req->find(e.parent->name);
			} else
				req = req->find(e.name);
			if(!req) {
				if(!value)
					warning("Can't find requisit \"%1\" to load value %2", e.name, value);
				else
					warning("Can't find requisit \"%1\"", e.name);
				e.skip = true;
				return 0;
			}
			auto i = 0;
			if(e.parent->type == serializer::kind::Array)
				i = e.index;
			if(i >= (int)req->count)
				i = req->count - 1;
			if(e.parent->type == serializer::kind::Array)
				e.object = req->ptr(e.parent->parent->object, i);
			else
				e.object = req->ptr(e.parent->object);
			e.metadata = (void*)req->type;
			if(e.type != serializer::kind::Struct)
				testslice(req, e);
			return req;
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
				readcontext(e, 0);
				break;
			}
		}
		void close(serializer::node& e) override {
		}
		bool read(variant& v, const char* value) {
			v = value;
			if(!v) {
				warning("Can't find variant \"%1\"", value);
				return false;
			}
			return true;
		}
		void save(const bsreq* type, unsigned size, void* object, const char* value) {
			if(type == bsmeta<flagable<1>>::meta
				|| type == bsmeta<flagable<2>>::meta
				|| type == bsmeta<flagable<4>>::meta) {
				variant v;
				if(read(v, value)) {
					auto ps = (unsigned char*)object;
					ps[v.value / 8] |= (1 << (v.value % 8));
				}
			} else if(type == bsmeta<variant>::meta) {
				variant v;
				if(read(v, value)) {
					auto ps = (variant*)object;
					*ps = v;
				}
			} else if(type == bsmeta<int>::meta) {
				auto numberic = stringbuilder::getnum(value);
				switch(size) {
				case 1: *((char*)object) = numberic; break;
				case 2: *((short*)object) = numberic; break;
				case 8: *((char*)object) = numberic; break;
				default: *((int*)object) = numberic; break;
				}
			} else if(type == bsmeta<const char*>::meta)
				*((const char**)object) = szdup(value);
		}
		void set(serializer::node& e, const char* value) override {
			auto req = readcontext(e, value);
			if(!req)
				return;
			save(req->type, req->size, e.object, value);
		}
		proxy() {
		}
	};
	error_url = url;
	proxy reader_proxy;
	return io::read(url, reader_proxy) && log::geterrors() == 0;
}