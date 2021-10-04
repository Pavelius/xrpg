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

static const char*	current_url;
static int			error_count;

static int szlinenumber(const char* start, const char* pv) {
	auto result = 1;
	for(auto p = start; *p && p < pv; p++) {
		if(*p == 10 || *p == 13) {
			result++;
			p = skipcr(p);
		}
	}
	return result;
}

static void warning(const char* p, const char* p_alloc, const char* format, ...) {
	error_count++;
	if(current_url) {
		log::error("Errors occurs when reading events in %1", current_url);
		current_url = 0;
	}
	char temp[4096]; stringbuilder sb(temp);
	sb.add("In line %1i ", szlinenumber(p_alloc, p));
	sb.addv(format, xva_start(format));
	log::errorv(temp);
}

static const char* read_block(const char* p, const char* p_alloc, int& v, const char*& text, variants& consequences) {
	char temp[8192];
	v = 0; text = 0; temp[0] = 0; consequences.clear();
	if(!isnum(p[0])) {
		warning(p, p_alloc, "Expected number");
		return 0;
	}
	p = stringbuilder::read(p, v);
	p = skipsp(p);
	while(*p && !(*p=='\n' || *p=='\r')) {
		if(!ischa(*p)) {
			return p;
		}
		auto pid = p;
		stringbuilder sb(temp);
		p = sb.psidf(p);
		p = skipsp(p);
		variant v = (const char*)temp;
		if(!v) {
			warning(pid, p_alloc, "Can't find variant `%1`", temp);
			return 0;
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
	current_url = url;
	error_count = 0;
	auto p = p_alloc;
	variants resolves;
	const auto maximum_index = sizeof(eventi::actions) / sizeof(eventi::actions[0]);
	while(isheader(p)) {
		int stage = 0;
		const char* text = 0;
		p = read_block(p + 1, p_alloc, stage, text, resolves);
		if(!p)
			break;
		auto pi = (eventi*)source.ptr(stage);
		pi->clear();
		pi->text = text;
		if(!isanswer(p)) {
			warning(p, p_alloc, "Expected `~1` token");
			break;
		}
		p = read_block(p + 1, p_alloc, stage, pi->case1, resolves);
		if(!p)
			break;
		if(!isanswer(p)) {
			warning(p, p_alloc, "Expected `~2` token");
			break;
		}
		p = read_block(p + 1, p_alloc, stage, pi->case2, resolves);
		auto index = 0;
		while(p && isresolve(p)) {
			if(index >= maximum_index) {
				warning(p, p_alloc, "Resolution part have more %1i lines", maximum_index);
				break;
			}
			auto& e = pi->actions[index++];
			p = read_block(p + 2, p_alloc, e.stage, e.text, e.consequences);
		}
		if(!p)
			break;
	}
	delete p_alloc;
	return error_count==0;
}