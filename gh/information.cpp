#include "main.h"

static void addh(stringbuilder& sb, const char* title) {
	sb.addn("##%1", title);
}

//static void add(stringbuilder& sb, variant a, const char* prefix) {
//	sb.add(",");
//	sb.adds(prefix);
//	sb.adds(a.getname());
//}
//
//static void add(stringbuilder& sb, variant a, int b, bool add_sep = true) {
//	if(add_sep)
//		sb.add(", ");
//	if(b)
//		sb.add("%1 %2i", a.getname(), b);
//	else if(a.type == SummonedCreature)
//		sb.add("%Summon %1", a.getname());
//	else
//		sb.add(a.getname());
//}
//
//static void add_area(stringbuilder& sb, area_s a, int b) {
//	sb.add(", ");
//	sb.add("%1 %2i", getnm(bsdata<areai>::get(a).id), b);
//}
//
//static void addpart2(stringbuilder& sb, const variants& source) {
//	scripti e;
//	auto pe = source.end();
//	for(auto p = (const variant*)source.begin(); p < pe; ) {
//		e.clear();
//		p = e.apply(p, pe);
//		sb.addn("* ");
//		if(e.duration) {
//			sb.add(getnm(bsdata<durationi>::get(e.duration).id), e.duration_lenght);
//			sb.add(": ");
//		}
//		add(sb, e.action, e.bonus, false);
//		if(e.range)
//			add(sb, Range, e.range);
//		if(e.area)
//			add_area(sb, e.area, e.area_size);
//		if(e.targets)
//			add(sb, Target, e.targets);
//		if(e.pierce)
//			add(sb, Pierce, e.pierce);
//		if(e.experience)
//			add(sb, Experience, e.experience);
//		for(auto i = 0; i <= AnyElement; i++) {
//			if(e.elements.is(i))
//				add(sb, variant(Element, i), 0);
//		}
//		for(auto i = 0; i <= AnyElement; i++) {
//			if(e.elements_use.is(i))
//				add(sb, variant(Element, i), "Если");
//		}
//		for(auto i = 0; i < NextAction; i++) {
//			if(e.feats.is(i))
//				add(sb, variant(Feat, i), 0);
//		}
//	}
//}

static void addpart(stringbuilder& sb, const variants& source) {
	int count = 0;
	for(auto v : source) {
		auto a = scripti::getaction(v);
		auto b = scripti::getbonus(v);
		if(a.type==Duration) {
			sb.addn("* ");
			sb.add(a.getname(), b);
			sb.add(": ");
			count = -1;
			continue;
		} else if(count < 0)
			count = 0;
		else if(!count)
			sb.addn("* ");
		else
			sb.add(", ");
		if(b) {
			if(a.type==Element && b<0)
				sb.add("Если %-1", a.getname());
			else
				sb.add("%1 %2i", a.getname(), b);
		} else if(a.type==SummonedCreature)
			sb.add("%Summon %1", a.getname());
		else
			sb.add(a.getname());
		count++;
	}
}

void cardi::getinfo(stringbuilder& sb) const {
	sb.clear();
	addh(sb, getnm(id));
	if(owner)
        sb.addn("%1, %Level %2i, %Initiative %3i", owner.getname(), level, initiative);
    auto need_header = upper && lower;
    if(need_header)
        sb.addn("[Верхняя]");
	addpart(sb, upper);
	if(need_header)
        sb.addn("[Нижняя]");
	addpart(sb, lower);
}

void object::getinfo(stringbuilder& sb) const {
	sb.clear();
	addh(sb, kind.getname());
}

void variant::getinfo(stringbuilder& sb) const {
	switch(type) {
	case Card: bsdata<cardi>::get(value).getinfo(sb); break;
	case Object: bsdata<object>::get(value).getinfo(sb); break;
	default: break;
	}
}
