#pragma once

#define HANDLER(T) static void proc_##T();\
static handler handler_##T(proc_##T, T);\
static void proc_##T()

typedef void (*fnevent)();

struct handler {
	fnevent		proc;
	handler*	next;
	handler(fnevent proc, handler*& first) : proc(proc), next(0) {
		if(!first)
			first = this;
		else {
			auto p = first;
			while(p->next)
				p = p->next;
			p->next = this;
		}
	}
	void execute() {
		for(auto p = this; p; p = p->next)
			p->proc();
	}
};