#include "answers.h"
#include "draw_input.h"
#include "requisit.h"
#include "menu.h"

static const char*	current_resid;

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

static void choose_source(answers& an, const char* title, const char* resid, variant source, fnmenuallow pallow = 0) {
	an.clear();
	fill_source(an, source, pallow);
	if(!an)
		return;
	auto push_resid = current_resid;
	if(resid)
		current_resid = resid;
	auto result = (void*)an.choose(getnm(title), 0, true, current_resid);
	current_resid = push_resid;
}

static void choose_menu_new(answers& an, const char* parent, const char* title, const char* type) {
	auto push_res = current_resid;
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
		if(equal(type, "Back"))
			cancel = getnm("Back");
		auto p = (menui*)an.choose(title, cancel, true, current_resid);
		if(!p)
			break;
		if(p->resid)
			current_resid = p->resid;
		auto proc = getcommand(p->id);
		if(proc)
			proc();
		if(equal(type, "Back"))
			choose_menu_new(an, p->id, getdescription(p->id), p->type);
		else if(equal(type, "StepByStep")) {
			for(auto& e : bsdata<menui>()) {
				if(strcmp(e.parent, parent) != 0)
					continue;
				choose_source(an, getdescription(p->id), p->resid, p->id, 0);
			}
		} else if(!type) {
			title = getdescription(p->id);
			type = p->type;
			parent = p->id;
		}
	}
	current_resid = push_res;
}

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
