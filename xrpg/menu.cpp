#include "answers.h"
#include "draw.h"
#include "requisit.h"
#include "menu.h"

variant			menu_result;
bool			menu_break;
const char*		menu_resid;
fnmenuallow		menu_allow;
fnmenuevent		menu_apply, menu_prepare;

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

static void choose_element(answers& an, const char* title, const char* parent, const char* type) {
	auto push_res = menu_resid;
	auto push_allow = menu_allow;
	variant source = type;
	if(source) {
		if(menu_prepare)
			menu_prepare(parent, type);
		auto proc = getcommand(parent);
		if(proc)
			proc();
		an.clear();
		fill_source(an, source, menu_allow);
		menu_result.clear();
		if(an) {
			menu_result = (void*)an.choose(title, 0, true, menu_resid);
			if(menu_apply)
				menu_apply(parent, type);
		}
	} else if(equal(type, "StepByStep")) {
		auto proc = getcommand(parent);
		if(proc)
			proc();
		for(auto& e : bsdata<menui>()) {
			if(strcmp(e.parent, parent) != 0)
				continue;
			if(e.resid)
				menu_resid = e.resid;
			choose_element(an, getdescription(e.id), e.id, e.type);
		}
	} else {
		while(parent && !draw::isnext()) {
			auto proc = getcommand(parent);
			if(proc)
				proc();
			an.clear();
			for(auto& e : bsdata<menui>()) {
				if(strcmp(e.parent, parent) != 0)
					continue;
				an.add((long)&e, getnm(e.id));
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
			if(equal(p->type, "StepByStep")) {
				choose_element(an, getdescription(p->id), p->id, p->type);
				break;
			} else {
				if(equal(type, "Submenu"))
					choose_element(an, getdescription(p->id), p->id, p->type);
				else {
					title = getdescription(p->id);
					parent = p->id;
					type = p->type;
				}
			}
		}
	}
	menu_allow = push_allow;
	menu_resid = push_res;
}

void menui::choose(const char* parent, const char* resid, const char* title) {
	answers an;
	menu_result.clear();
	menu_resid = resid;
	menu_break = false;
	choose_element(an, title, parent, 0);
}
