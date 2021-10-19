#include "log.h"
#include "main.h"

BSDATAD(eventi)
BSDATAD(eventcasei)

// 1) One/Two on none events occurs in province of each player. Each player have same cards deck. About 30-40 cards.
// 2) One event occurs in random province occuped by each player. This is separate deck and it is suppose to be bad.
// 3) You can mix to some part of player deck (or neutral deck) additional cards.

static const char* skipnum(const char* p) {
	while(isnum(*p))
		p++;
	return p;
}

static bool isanswer(const char* p) {
	auto p1 = skipnum(p);
	return p1 != p && p1[0] == ')';
}

static bool isevent(const char* p) {
	return p[0] == '#' && ischa(p[1]);
}

static bool iseventstage(const char* p) {
	return p[0] == '#' && isnum(p[1]);
}

static const char* read_string(const char* p, stringbuilder& result) {
	result.clear();
	while(*p) {
		char sym;
		if(*p == '\n' || *p == '\r') {
			p = skipspcr(p);
			if(p[0] == '#' || isanswer(p))
				break;
			sym = '\n';
		} else
			sym = *p++;
		switch(sym) {
		case 17: sym = '-'; break;
		}
		result.add(sym);
	}
	return p;
}

static void add(variants& e, variant v) {
	if(!e.count)
		e.start = bsdata<variant>::source.getcount();
	auto p = (variant*)bsdata<variant>::source.add();
	*p = v;
	e.count++;
}

static const char* read_identifier(const char* p, stringbuilder& result) {
	result.clear();
	return result.psidf(p);
}

static const char* read_variants(const char* p, stringbuilder& result, variants& source, bool& allow_continue) {
	while(allow_continue && ischa(*p)) {
		p = read_identifier(p, result);
		p = skipsp(p);
		variant v = (const char*)result.begin();
		if(!v)
			log::error(p, "Can't find variant `%1`", result.begin());
		add(source, v);
	}
	return p;
}

static short add_event(const char* p, const char* uid) {
	for(auto& e : bsdata<eventi>()) {
		if(e.id == uid) {
			log::error(p, "Find duplicate event `%1`", uid);
			return bsdata<eventi>::source.indexof(&e);
		}
	}
	auto pe = bsdata<eventi>::addz();
	pe->id = uid;
	return bsdata<eventi>::source.indexof(pe);
}

static const char* skipcr(const char* p, bool& allow_continue) {
	if(p[0] == 10 || p[0] == 13)
		p = skipspcr(p);
	else {
		log::error(p, "Expected line feed");
		allow_continue = false;
	}
	return p;
}

static const char* read_answers(const char* p, short parent, short id, stringbuilder& sb, bool& allow_continue) {
	while(allow_continue && isanswer(p)) {
		auto pe = bsdata<eventcasei>::add();
		pe->clear();
		pe->parent = parent;
		pe->id = id;
		p = stringbuilder::read(p, pe->next);
		p = skipsp(p + 1);
		p = read_string(p, sb);
		pe->text = szdup(sb.begin());
	}
	return p;
}

static const char* read_stage(const char* p, short parent, stringbuilder& sb, bool& allow_continue) {
	while(allow_continue && iseventstage(p)) {
		auto pe = bsdata<eventcasei>::add();
		pe->clear();
		pe->parent = parent;
		pe->next = -1;
		p = stringbuilder::read(skipsp(p + 1), pe->id);
		p = read_variants(skipsp(p), sb, pe->effect, allow_continue);
		p = read_string(skipspcr(p), sb);
		pe->text = szdup(sb.begin());
		p = read_answers(p, parent, pe->id, sb, allow_continue);
	}
	return p;
}

void eventcasei::clear() {
	memset(this, 0, sizeof(*this));
}

void eventi::read(const char* url) {
	auto p_alloc = (const char*)loadt(url);
	if(!p_alloc)
		return;
	log::seturl(url);
	auto p = p_alloc;
	log::setfile(p);
	variants resolves;
	char temp[4096]; stringbuilder sb(temp);
	auto allow_continue = true;
	while(allow_continue && p[0]) {
		if(!isevent(p)) {
			log::error(p, "Expected symbol `#` and followed event identifier");
			break;
		}
		auto p1 = p + 1;
		p = read_identifier(p1, sb);
		auto event_parent = add_event(p1, szdup(temp));
		auto event_id = 0;
		p = skipcr(p, allow_continue);
		p = read_string(p, sb);
		auto pe = bsdata<eventcasei>::add();
		pe->clear();
		pe->parent = event_parent;
		pe->id = event_id;
		pe->next = -1;
		pe->text = szdup(temp);
		if(!isanswer(p)) {
			log::error(p, "Expected number of answer followed by symbol `)`");
			allow_continue = false;
		}
		p = read_answers(p, pe->parent, pe->id, sb, allow_continue);
		p = read_stage(p, pe->parent, sb, allow_continue);
	}
	delete p_alloc;
}

const eventi* eventi::find(const char* id) {
	for(auto& e : bsdata<eventi>()) {
		if(strcmp(e.id, id) == 0)
			return &e;
	}
	return 0;
}