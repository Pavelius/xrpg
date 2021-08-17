#include "cflags.h"
#include "code.h"
#include "string.h"

enum flags_s : unsigned char {
	Variable, Condition, Repeat
};
enum class error : unsigned char {
	ExpectedP1,
};
typedef cflags<flags_s> tokenf;
struct tokeni {
	const char*			id;
	unsigned			flags;
	const struct rulei*	rule;
	constexpr tokeni() : flags(), id(0), rule(0) {}
	constexpr tokeni(const char* p) : flags(), id(p), rule(0) {
		while(*p) {
			if(*p == '\\') {
				id = p + 1;
				break;
			} else if(*p == '.')
				set(Repeat);
			else if(*p == '%')
				set(Variable);
			else if(*p == '?')
				set(Condition);
			else {
				id = p;
				break;
			}
			p++;
		}
	}
	constexpr explicit operator bool() const { return id != 0; }
	constexpr bool		is(flags_s v) const { return (flags & (1 << v)) != 0; }
	void				parse() const;
	constexpr void		set(flags_s v) { flags |= 1 << v; }
};
typedef tokeni tokena[16];
struct rulei {
	tokeni				name;
	tokena				tokens;
	fnevent				special;
	void				parse() const;
};
typedef slice<rulei>	rulea;
struct corei {
	string				id, rule, url, comment;
};

static const char*		p;
static rulea			this_rules;
static int				this_errors;
static corei			core;

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
			if(e.is(Variable))
				e.rule = find_rule(e.id);
		}
	}
}

static bool ischab(char sym) {
	return (sym >= 'A' && sym <= 'Z')
		|| (sym >= 'a' && sym <= 'z')
		|| (sym == '_');
}

static void skipws() {
	p = skipspcr(p);
}

static void errors(error id, ...) {
	this_errors++;
}

static void identifier() {
	if(!ischab(*p))
		return;
	auto pb = p;
	while(ischa(*p))
		p++;
	core.id.set(pb, p - pb);
	skipws();
}

static void add_type() {
}

static void add_member() {
}

static void set_url() {
	core.url = core.rule;
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
	{"?global", {"%import", "%variable", "%function"}},
	{"import", {"import", "%url", "?%as_id", ";"}, add_type},
	{"as_id", {"as", "%id"}},
	{"url", {"%id", "?%.trail_id"}, set_url},
	{"trail_id", {"\\.", "%id"}},
	{"id", {}, identifier},
};

void rulei::parse() const {
	auto pb = p;
	for(auto& e : tokens) {
		if(!e)
			break;
		auto p1 = p;
		e.parse();
		if(e.is(Repeat)) {
			auto p2 = p1;
			while(p2 != p) {
				p2 = p;
				e.parse();
			}
		}
		if(name.is(Condition) && p1 != p) {
			// This token is match and only one in list must be valid
			break;
		}
		if(e.is(Condition))
			// If tokens is optional this is not error case.
			continue;
		if(p1 == p) {
			// This token is not match.
			if(pb != p)
				// Some of previous tokens match. Error case
				errors(error::ExpectedP1, e.id);
			break;
		}
	}
	if(p != pb) {
		core.rule.set(pb, p - pb);
	}
	if(special)
		special();
}

void tokeni::parse() const {
	if(rule)
		// This is rule, not token
		rule->parse();
	else {
		// This is determinal, that must match input string
		auto n = zlen(id);
		if(memcmp(p, id, n) != 0)
			return;
		p += n;
		skipws();
	}
}

static const char* code_sample = "\
import core.crt;\n\
import core.collection.array as this;\n\
";

void initialize_complex_grammar() {
	this_rules = c2_grammar;
	update_rules();
	auto pr = find_rule("global");
	if(!pr)
		return;
	p = code_sample;
	while(*p) {
		auto pb = p;
		pr->parse();
		if(pb == p)
			break;
	}
}