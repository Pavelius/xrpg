#include "command.h"
#include "crt.h"

const command* command::find(unsigned v) const {
	for(auto p = this; *p; p++)
		if(p->key == v)
			return p;
	return 0;
}
const command* command::find(const char* v) const {
	for(auto p = this; *p; p++)
		if(strcmp(p->id, v)==0)
			return p;
	return 0;
}