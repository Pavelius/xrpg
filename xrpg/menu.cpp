#include "answers.h"
#include "menu.h"
#include "view.h"

static void choose_menu(answers& an, const char* parent, const char* resid, const char* title, menui::flaga flags) {
	while(parent) {
		an.clear();
		for(auto& e : bsdata<menui>()) {
			if(strcmp(e.parent, parent) != 0)
				continue;
			an.add((long)&e, e.name);
		}
		const char* cancel = 0;
		if(flags.is(MenuBack))
			cancel = "Назад";
		auto p = (menui*)an.choose(title, cancel, true, resid);
		if(!p)
			parent = 0;
		else if(p->flags.is(MenuBack)) {
			if(p->result.proc)
				p->result.proc();
			choose_menu(an, p->result.next, p->resid, p->text, p->flags);
		} else {
			parent = p->result.next;
			if(p->result.proc) {
				if(!p->result.next)
					draw::setnext(p->result.proc);
				else
					p->result.proc();
			}
			title = p->text;
			if(p->resid)
				resid = p->resid;
			flags = p->flags;
		}
	}
}

void menui::choose(const char* parent, const char* resid, const char* title) {
	answers an;
	choose_menu(an, parent, resid, title, {});
}
