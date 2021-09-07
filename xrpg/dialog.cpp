#include "crt.h"
#include "dialog.h"
#include "io_stream.h"
#include "variant.h"

BSDATAD(dialogi)
BSDATAD(speechi)

static dialogi* find_dialog(const char* id) {
	for(auto& e : bsdata<dialogi>()) {
		if(strcmp(e.id, id) == 0)
			return &e;
	}
	return 0;
}

static const char* read_string(const char* p, char* ps, const char* pe) {
	auto pb = ps;
	while(*p) {
		if((p[0] == 13 || p[0] == 10) && (p[1] == '-' || p[1] == '#')) {
			p = skipspcr(p);
			break;
		}
		if(*p == '\r') {
			p++;
			continue;
		}
		if(ps < pe)
			*ps++ = *p;
		p++;
	}
	while(ps > pb && ps[-1] == '\n')
		ps--;
	*ps = 0;
	return p;
}

static const char* read_identifier(const char* p, int& id, sliceu<variant>& commands) {
	char name[32]; stringbuilder sb(name);
	if(isnum(*p))
		p = stringbuilder::read(p, id);
	else {
		sb.clear();
		p = sb.psidf(p);
		id = (int)szdup(name);
	}
	p = skipsp(p);
	adat<variant, 128> source;
	while(p[0] == ',') {
		sb.clear();
		p = skipsp(p + 1);
		p = sb.psidf(p);
		p = skipsp(p);
		variant result = name;
		source.add(result);
	}
	commands.set(source.data, source.count);
	return p;
}

static bool read_dialog(const char* url) {
	char value[8192]; szfnamewe(value, url);
	auto p_alloc = (const char*)loadt(url);
	if(!p_alloc)
		return false;
	auto pdg = find_dialog(value);
	if(!pdg) {
		pdg = bsdata<dialogi>::add();
		memset(pdg, 0, sizeof(*pdg));
		pdg->id = szdup(value);
	}
	auto pid = bsdata<dialogi>::source.indexof(pdg);
	auto p = p_alloc;
	auto records_read = 0;
	speechi* pc_base = 0;
	while(*p) {
		auto pc = bsdata<speechi>::add();
		memset(pc, 0, sizeof(*pc));
		pc->parent = pid;
		if(p[0] == '#') {
			pc_base = pc;
			pc->next_index = 0xFFFFFFFF;
			p = read_identifier(p + 1, pc->index, pc->commands);
		} else if(p[0] == '-') {
			if(pc_base)
				pc->index = pc_base->index;
			p = read_identifier(p + 1, pc->next_index, pc->commands);
		}
		if(*p == ':')
			p = skipspcr(p + 1);
		else
			return false;
		p = read_string(p, value, value + sizeof(value) - 1);
		pc->format = szdup(value);
		records_read++;
	}
	delete p_alloc;
	return true;
}

bool initialize_speech(const char* locale) {
	char temp[128]; stringbuilder sb(temp);
	sb.add("chats/%1", locale);
	auto result = true;
	for(io::file::find find(temp); find; find.next()) {
		auto pn = find.name();
		if(pn[0] == '.')
			continue;
		if(!szpmatch(pn, "*.txt"))
			continue;
		char fullname[260]; find.fullname(fullname);
		if(!read_dialog(fullname))
			result = false;
	}
	return result;
}