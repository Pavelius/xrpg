#include "answers.h"
#include "draw_input.h"
#include "menu.h"

static void choose_menu(answers& an, const char* parent, const char* resid, const char* title, bool allow_cancel) {
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
		if(allow_cancel)
			cancel = getnm("Back");
		auto p = (menui*)an.choose(title, cancel, true, resid);
		if(!p)
			parent = 0;
		else if(allow_cancel) {
			auto proc = getcommand(p->id);
			if(proc)
				proc();
			choose_menu(an, p->id, p->resid, getdescription(p->id), p->menuback);
		} else {
			auto proc = getcommand(p->id);
			parent = p->id;
			if(proc)
				draw::setnext(proc);
			title = getdescription(p->id);
			if(p->resid)
				resid = p->resid;
			allow_cancel = p->menuback;
		}
	}
}

void menui::choose(const char* parent, const char* resid, const char* title) {
	answers an;
	choose_menu(an, parent, resid, title, {});
}
