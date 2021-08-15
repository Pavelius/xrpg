#include "code.h"
#include "string.h"

using namespace code;

static const lexer*	current_parser;
static pointl		current_pos;
static group_s		current_group;
static unsigned		current_index, current_forward;
static const char*	current_url;
static const char*	p;

void typei::clear() {
	memset(this, 0, sizeof(*this));
}

void memberi::clear() {
	memset(this, 0, sizeof(*this));
}

static void next() {
	while(*p) {
		auto pb = p;
		p = getnext(pb, current_pos, current_group, current_parser);
		if(current_group == Comment || current_group == WhiteSpace)
			continue;
		break;
	}
}

static typei* findtype(const char* id) {
	for(auto& e : bsdata<typei>()) {
		if(!e)
			continue;
		if(strcmp(e.id, id) == 0)
			return &e;
	}
	return 0;
}

static memberi* findmember(const char* id, const char* type) {
	for(auto& e : bsdata<memberi>()) {
		if(!e)
			continue;
		if(strcmp(e.id, id) == 0
			&& strcmp(e.type, type) == 0)
			return &e;
	}
	return 0;
}

static typei* addtype(const char* id) {
	auto p = findtype(id);
	if(!p)
		p = bsdata<typei>::add();
	p->id = szdup(id);
	p->url = current_url;
	return p;
}

static memberi* addmember(const char* id, const char* type, const char* result) {
	auto p = findmember(id, type);
	if(!p)
		p = bsdata<memberi>::add();
	p->id = szdup(id);
	p->url = current_url;
	p->type = szdup(type);
	p->result = szdup(result);
	return p;
}

static void block() {

}

void code::parse(const char* url, const char* source, const lexer* px) {
	p = source;
	current_parser = px;
	while(*p) {
		next();
	}
}