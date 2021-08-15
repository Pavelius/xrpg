#include "code.h"

using namespace code;

static const lexer*	current_parser;
static pointl		current_pos;
static group_s		current_group;
static const char*	p;

static void next() {
	while(*p) {
		auto pb = p;
		p = getnext(pb, current_pos, current_group, current_parser);
		if(current_group == Comment || current_group == WhiteSpace)
			continue;
		break;
	}
}

void code::parse(const char* source, const lexer* px) {
	p = source;
	current_parser = px;
	while(*p) {
		next();
		switch(current_group) {
		case Keyword:
			break;
		}
	}
}