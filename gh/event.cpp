#include "main.h"
#include "log.h"

static eventi city_events_data[200];
array city_events(city_events_data, sizeof(city_events_data[0]), 0, sizeof(city_events_data) / sizeof(city_events_data[0]));

static array road_events_data[200];
array road_events(road_events_data, sizeof(road_events_data[0]), 0, sizeof(road_events_data) / sizeof(road_events_data[0]));

void eventi::clear() {
	memset(this, 0, sizeof(*this));
}

static bool isheader(const char* p) {
	return p[0] == '#' && isnum(p[1]);
}

static bool isanswer(const char* p) {
	return p[0] == '~' && isnum(p[1]);
}

static bool isresolve(const char* p) {
	return p[0] == '-' && p[1] == '-' && isnum(p[2]);
}

static const char* read_string(const char* p, char* ps, const char* pe) {
	char sym;
	while(*p) {
		if(*p == '\n' || *p == '\r') {
			p = skipcr(p);
			p = skipsp(p);
			if(isheader(p) || isanswer(p) || isresolve(p))
				break;
			sym = '\n';
		} else
			sym = *p++;
		switch(sym) {
		case 17: sym = '-'; break;
		}
		if(ps < pe)
			*ps++ = sym;
	}
	*ps = 0;
	return p;
}

static void add(variants& e, variant v) {
	if(!e.count)
		e.start = bsdata<variant>::source.getcount();
	auto p = (variant*)bsdata<variant>::source.add();
	*p = v;
	e.count++;
}

static const char* read_block(const char* p, const char* p_alloc, int& v, const char*& text, variants& consequences) {
	char temp[8192];
	v = 0; text = 0; temp[0] = 0; consequences.clear();
	if(!isnum(p[0])) {
		log::error(p, "Expected number");
		return p;
	}
	p = stringbuilder::read(p, v);
	p = skipsp(p);
	while(*p && !(*p=='\n' || *p=='\r')) {
		if(!ischa(*p)) {
			return p;
		}
		stringbuilder sb(temp);
		p = sb.psidf(p);
		p = skipsp(p);
		variant v = (const char*)temp;
		if(!v) {
			log::error(p, "Can't find variant `%1`", temp);
			return p;
		}
		add(consequences, v);
	}
	p = skipspcr(p);
	p = read_string(p, temp, temp + sizeof(temp) - 1);
	text = szdup(temp);
	return p;
}

bool eventi::read(const char* url, array& source) {
	auto p_alloc = (const char*)loadt(url);
	if(!p_alloc)
		return false;
	log::seturl(url);
	auto p = p_alloc;
	variants resolves;
	const int maximum_index = sizeof(eventi::actions) / sizeof(eventi::actions[0]);
	int maximum_stage = 0;
	int minimum_stage = 100000;
	while(isheader(p)) {
		int stage = 0;
		const char* text = 0;
		p = read_block(p + 1, p_alloc, stage, text, resolves);
		if(maximum_stage < stage)
            maximum_stage = stage;
		if(minimum_stage > stage)
            minimum_stage = stage;
		auto pi = (eventi*)source.ptr(stage);
		pi->clear();
		pi->text = text;
		if(!isanswer(p)) {
			log::error(p, "Expected `~1` token");
			break;
		}
		p = read_block(p + 1, p_alloc, stage, pi->case1, resolves);
		if(!isanswer(p)) {
			log::error(p, "Expected `~2` token");
			break;
		}
		p = read_block(p + 1, p_alloc, stage, pi->case2, resolves);
		auto index = 0;
		while(isresolve(p)) {
			if(index >= maximum_index) {
				log::error(p, "Resolution part have more %1i lines", maximum_index);
				break;
			}
			auto& e = pi->actions[index++];
			p = read_block(p + 2, p_alloc, e.stage, e.text, e.consequences);
		}
        if(!pi->case1)
            log::error(p, "Not found answer #1");
        if(!pi->case2)
            log::error(p, "Not found answer #2");
	}
	if((int)source.getcount() < maximum_stage)
        source.setcount(maximum_stage + 1);
	delete p_alloc;
	return true;
}