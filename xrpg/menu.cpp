#include "answers.h"
#include "draw_input.h"
#include "menu.h"

static const char* current_resid;

static void choose_menu(answers& an, const char* parent, const char* title, bool allow_cancel) {
	while(parent && !draw::isnext()) {
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
		auto p = (menui*)an.choose(title, cancel, true, current_resid);
		if(!p)
			break;
		if(p->resid)
			current_resid = p->resid;
		auto proc = getcommand(p->id);
		if(proc)
			proc();
		if(allow_cancel) {
			auto push_res = current_resid;
			choose_menu(an, p->id, getdescription(p->id), p->menuback);
			current_resid = push_res;
		} else {
			title = getdescription(p->id);
			allow_cancel = p->menuback;
			parent = p->id;
		}
	}
}

void menui::choose(const char* parent, const char* resid, const char* title) {
	answers an;
	current_resid = resid;
	choose_menu(an, parent, title, true);
}
