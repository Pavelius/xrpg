#include "answers.h"
#include "draw_input.h"
#include "menu.h"

static void choose_menu(answers& an, const char* parent, const char* resid, const char* title, unsigned flags) {
	while(parent) {
		an.clear();
		for(auto& e : bsdata<menui>()) {
			if(strcmp(e.parent, parent) != 0)
				continue;
			an.add((long)&e, getnm(e.id));
		}
		if(!an.getcount())
			break;
		const char* cancel = 0;
		if((flags&(1<<MenuBack))!=0)
			cancel = getnm("Back");
		auto p = (menui*)an.choose(title, cancel, true, resid);
		if(!p)
			parent = 0;
		else if((p->flags & (1 << MenuBack)) != 0) {
			if(p->proc)
				p->proc();
			choose_menu(an, p->id, p->resid, getdescription(p->id), p->flags);
		} else {
			parent = p->id;
			if(p->proc)
				draw::setnext(p->proc);
			title = getdescription(p->id);
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
