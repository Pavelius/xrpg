#include "crt.h"
#include "stringbuilder.h"
#include "tablecsv.h"
#include "variant.h"

static array source(sizeof(int));
BSDATAD(tablecsvi)

const int* tablecsvi::begin() const {
	return (int*)source.ptr(start);
}

const int* tablecsvi::end() const {
	return (int*)source.ptr(start + count);
}

int tablecsvi::getrows() const {
	if(!column_count)
		return 0;
	return count / column_count;
}

int	tablecsvi::get(int v) const {
	if(!count)
		return 0;
	auto m = getrows();
	if(v > m)
		v = m;
	return begin()[v];
}

static const char* getfileid(const char* url) {
	char temp[512]; stringbuilder sb(temp); sb.add(url);
	auto pf = szfname(temp);
	auto pe = (char*)szext(temp);
	if(pe)
		pe[-1] = 0;
	return szdup(pf);
}

bool tablecsvi::read(const char* url) {
	auto pf = loadt(url);
	if(!pf)
		return false;
	auto id = getfileid(url);
	auto pn = (tablecsvi*)bsdata<tablecsvi>::source.ptrs(bsdata<tablecsvi>::source.find(id, 0));
	if(!pn) {
		pn = bsdata<tablecsvi>::add();
		memset(pn, 0, sizeof(*pn));
		pn->id = id;
	}
	auto result = pn->parse(pf);
	delete pf;
	return result;
}

static const char* read_columns(const char* p, short unsigned& count) {
	char temp[260]; stringbuilder sb(temp);
	while(*p) {
		if(*p == '\"' || *p == '\'') {
			auto stop_symbol = *p++;
			sb.clear();
			p = sb.psstr(p, stop_symbol);
		} else if(isnum(*p)) {
			int v = 0;
			p = sb.read(p, v);
		} else if(ischa(*p)) {
			sb.clear();
			p = sb.psidf(p);
		}
		count++;
		p = skipsp(p);
		if(p[0] == ',') {
			p = skipsp(p + 1);
			continue;
		} else if(*p == '\n' || *p == '\r')
			p = skipcr(p);
		break;
	}
	return p;
}

bool tablecsvi::parse(const char* p) {
	static int temp_int[8192];
	auto ps = temp_int;
	auto pe = temp_int + sizeof(temp_int) / sizeof(temp_int[0]);
	char temp[260]; stringbuilder sb(temp);
	p = read_columns(p, column_count);
	short unsigned index = 0;
	short unsigned lines = 0;
	while(*p) {
		if(*p == '\"' || *p == '\'') {
			auto stop_symbol = *p++;
			sb.clear();
			p = sb.psstr(p, stop_symbol);
			variant v((const char*)temp);
			if(ps < pe)
				*ps++ = v;
		} else if(isnum(*p)) {
			int v = 0;
			p = sb.read(p, v);
			if(ps < pe)
				*ps++ = v;
		} else if(*p == ',') {
			if(ps < pe)
				*ps++ = 0;
		} else if(ischa(*p)) {
			sb.clear();
			p = sb.psidf(p);
			variant v((const char*)temp);
			if(ps < pe)
				*ps++ = v;
		}
		index++;
		p = skipsp(p);
		if(p[0] == ',') {
			p = skipsp(p + 1);
			continue;
		} else if(*p == '\n' || *p == '\r') {
			lines++;
			p = skipcr(p);
			while(index < column_count) {
				if(ps < pe)
					*ps++ = 0;
				index++;
			}
			index = 0;
		}
	}
	auto total_count = lines * column_count;
	auto p1 = source.addu(temp_int, total_count);
	if(!p1)
		return false;
	start = source.indexof(p1);
	count = total_count;
	return true;
}