#include "cflags.h"
#include "code.h"
#include "string.h"

enum flags_s : unsigned char {
	Variable, Condition, Repeat
};
enum class error : unsigned char {
	ExpectedP1, ExpectedOneOfP1,
};
typedef bool (*fnparse)();
typedef cflags<flags_s> tokenf;
struct rulei;
struct tokeni {
	const char*			id;
	tokenf				flags;
	const rulei*		rule;
	constexpr tokeni() : flags(), id(0), rule(0) {}
	constexpr tokeni(const char* p) : flags(), id(p), rule(0) {
		while(*p) {
			if(*p == '.')
				flags.add(Repeat);
			else if(*p == '%')
				flags.add(Variable);
			else if(*p== '?')
				flags.add(Condition);
			else {
				id = p;
				break;
			}
			p++;
		}
	}
	constexpr explicit operator bool() const { return id != 0; }
	bool				parse() const;
};
typedef tokeni tokena[16];
struct rulei {
	tokeni				name;
	tokena				tokens;
	fnparse				special;
	bool				parse() const;
};
typedef slice<rulei>	rulea;

static const char*		p;
static rulea			this_rules;
static int				this_errors;
static string			this_id;

static rulei* find_rule(const char* id) {
	for(auto& e : this_rules) {
		if(strcmp(e.name.id, id) == 0)
			return &e;
	}
	return 0;
}

static void update_rules() {
	for(auto& r : this_rules) {
		for(auto& e : r.tokens) {
			if(!e)
				break;
			if(e.flags.is(Variable))
				e.rule = find_rule(e.id);
		}
	}
}

static bool ischab(char sym) {
	return (sym >= 'A' && sym <= 'Z')
		|| (sym >= 'a' && sym <= 'z')
		|| (sym == '_');
}

static const char* skip(const char* p) {
	return skipspcr(p);
}

static bool perror(error id, ...) {
	this_errors++;
	return false;
}

static bool identifier() {
	if(!ischab(*p))
		return false;
	auto pb = p;
	while(ischa(*p))
		p++;
	this_id.set(pb, p - pb);
	p = skip(p);
	return true;
}

// Example grammatic visualization
// -------------------------------
// id		read identifier. Can test if this is identifier.
// number	read number. Like 0x12, 9, 05.
// literal	read string literal. Like "Simple string literal"
// %		next defenition marker. After this symbol in pseudocode lead variable
// ...		can repeat leading leateral
// -------------------------------
// Case types: Conditional, Pseudocode
// -------------------------------
// file : %global...
// global : %import | %variable | %function
// import type(url, last_id): import %url [as %id];
// url : %id [.%id...]
// variable member(parent, id, type): %type %id [=%initialization];
// function member(parent, id, type): %type %id ([%parameters]) %block_statements
// statements : %block_statements | %statement
// block_statements : { [%statement...] }
// statement : %statement_content;
// statement_content : %local | %assignment | %calling | %if | %while | %for | %return | %break | %continue
// while : while(%expression) %statements
// if : if(%expression) %statements [else %statements]
// for : for(%declaration;%expression;%expression) %statements

static rulei c2_grammar[] = {
	{"file", {".%global"}},
	{"?global", {"%import", "%variable", "%function"}},
	{"import", {"import", "%url", "?%as_id"}},
	{"as_id", {"as", "%id"}},
	{"url", {"%id", "?%trail_id"}},
	{"trail_id", {".", "%id"}},
	{"id", {}, identifier},
};

bool rulei::parse() const {
	if(special)
		return special();
	else if(name.flags.is(Condition)) {
		for(auto& e : tokens) {
			if(!e)
				break;
			if(e.parse())
				return true;
		}
		return perror(error::ExpectedOneOfP1, name.id);
	} else {
		for(auto& e : tokens) {
			if(!e)
				break;
			if(!e.parse()) {
				if(e.flags.is(Condition))
					continue;
				return perror(error::ExpectedP1, name.id);
			}
		}
		return true;
	}
}

bool tokeni::parse() const {
	if(rule)
		// This is rule, not token
		return rule->parse();
	else {
		// This is determinal, that must match input string
		auto n = zlen(id);
		if(memcmp(p, id, n) != 0)
			return false;
		p = skip(p + n);
		return true;
	}
}

static const char* code_sample = "\
import core.crt;\n\
import core.string as this;\n\
";

void initialize_complex_grammar() {
	this_rules = c2_grammar;
	update_rules();
	auto pr = find_rule("global");
	if(!pr)
		return;
	p = code_sample;
	while(*p) {
		if(!pr->parse())
			break;
	}
}