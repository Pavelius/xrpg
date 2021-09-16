#include "answers.h"
#include "draw_input.h"
#include "requisit.h"
#include "menu.h"

bool menu_break;
const char*	menu_resid;

static void fill_array(answers& an, array* source, fnmenuallow pallow) {
	const char* pe = source->end();
	for(auto pb = source->begin(); pb < pe; pb += source->size) {
		if(pallow) {
			if(!pallow(pb))
				continue;
		}
		auto id = *((const char**)pb);
		an.add((long)pb, getnm(id));
	}
}

static void fill_records(answers& an, variant source, fnmenuallow pallow) {
	for(auto& e : bsdata<requisiti>()) {
		if(e.parent != source)
			continue;
		if(pallow) {
			if(!pallow(&e))
				continue;
		}
		an.add((long)&e, getnm(e.id));
	}
}

static void fill_source(answers& an, variant source, fnmenuallow pallow) {
	auto& ei = bsdata<varianti>::begin()[source.type];
	if(ei.source == bsdata<varianti>::source_ptr)
		fill_array(an, bsdata<varianti>::elements[source.value].source, pallow);
	else if(ei.source == bsdata<requisiti>::source_ptr)
		fill_records(an, ((requisiti*)bsdata<requisiti>::source.ptr(source.value))->parent, pallow);
	else
		fill_records(an, source, pallow);
}

static void apply_result(const char* id, const char* requisit, variant result) {
}

static void choose_menu_new(answers& an, const char* parent, const char* title, const char* type) {
	auto push_res = menu_resid;
	while(parent && !draw::isnext()) {
		an.clear();
		variant source = type;
		if(source)
			fill_source(an, source, 0);
		else {
			for(auto& e : bsdata<menui>()) {
				if(strcmp(e.parent, parent) != 0)
					continue;
				an.add((long)&e, getnm(e.id));
			}
		}
		if(!an)
			break;
		const char* cancel = 0;
		if(equal(type, "Submenu"))
			cancel = getnm("Back");
		auto p = (menui*)an.choose(title, cancel, true, menu_resid);
		if(!p)
			break;
		if(p->resid)
			menu_resid = p->resid;
		auto proc = getcommand(p->id);
		if(proc)
			proc();
		if(equal(type, "Submenu") || equal(type, "SubmenuNoBack"))
			choose_menu_new(an, p->id, getdescription(p->id), p->type);
		else if(equal(type, "StepByStep")) {
			for(auto& e : bsdata<menui>()) {
				if(strcmp(e.parent, parent) != 0)
					continue;
				if(p->resid)
					menu_resid = p->resid;
				choose_menu_new(an, e.parent, getdescription(p->id), p->type);
			}
		} else {
			title = getdescription(p->id);
			type = p->type;
			parent = p->id;
		}
	}
	menu_resid = push_res;
}

void menui::choose(const char* parent, const char* resid, const char* title) {
	answers an;
	menu_resid = resid;
	menu_break = false;
	choose_menu_new(an, parent, title, 0);
}
