#include "code.h"
#include "string.h"

using namespace code;

static string		token;
static const lexer*	current_parser;
static pointl		current_pos;
static group_s		current_group;
static unsigned		current_index, current_forward;
static const char*	current_url;
static const char*	p;

BSDATAD(memberi)
BSDATAD(typei)

void typei::clear() {
	memset(this, 0, sizeof(*this));
}

void memberi::clear() {
	memset(this, 0, sizeof(*this));
}

static void next() {
	while(*p) {
		auto pb = p;
		p = getnext(p, current_pos, current_group, current_parser);
		if(current_group == Comment || current_group == WhiteSpace)
			continue;
		token.set(pb, p - pb);
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
		p = bsdata<typei>::addz();
	p->id = szdup(id);
	p->url = current_url;
	return p;
}

static memberi* addmember(const char* id, const char* type, const char* result) {
	auto p = findmember(id, type);
	if(!p)
		p = bsdata<memberi>::addz();
	p->id = szdup(id);
	p->url = current_url;
	p->type = szdup(type);
	p->result = szdup(result);
	return p;
}

static void block(const char* type, int level) {
	while(*p) {
		next();
		if(current_group == Keyword) {
			if(token == "fn") {
				next();
				if(current_group == Identifier)
					addmember(token.get(), type, 0);
			} else if(token == "struct") {
				next();
				if(current_group == Identifier) {
					auto name = token;
					addtype(token.get());
					next();
					if(current_group == BlockBegin)
						block(name.get(), level);
				}
			} else if(token == "impl") {
				next();
				if(current_group == Identifier) {
					auto name = token;
					next();
					if(current_group == BlockBegin)
						block(name.get(), level); // Same level. Can use function defenition.
				}
			}
		} else if(current_group == BlockEnd)
			break;
		else if(current_group == BlockBegin)
			block(type, level + 1);
	}
}

static void remove_previous() {
	for(auto& e : bsdata<typei>()) {
		if(e.url == current_url)
			e.clear();
	}
	for(auto& e : bsdata<memberi>()) {
		if(e.url == current_url)
			e.clear();
	}
}

void code::parse(const char* url, const char* source, const lexer* px) {
	p = source;
	current_parser = px;
	current_url = url;
	remove_previous();
	addtype("this");
	block("this", 0);
}