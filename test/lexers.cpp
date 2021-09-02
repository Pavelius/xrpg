#include "lexer.h"
#include "code_pack.h"

using namespace code;

static void add_type() {
	auto url = this_pack->add(core.type);
	auto id = this_pack->add(core.id);
	this_pack->addclass(id, url);
}

static void test_type() {
	core.type = core.id;
}

static void test_constant() {
}

static void set_url() {
	core.url = core.rule;
}

static void set_member() {
	core.member = core.id;
}

static void apply_static() {
}

static void apply_public() {
}

static void add_function() {
	auto id = this_pack->add(core.member);
	auto type = this_pack->add("void");
	this_pack->addsym(id, This, type, 0, 1 << static_cast<int>(symf::Method));
}

static void add_variable() {
	auto id = this_pack->add(core.member);
	auto type = this_pack->add("void");
	this_pack->addsym(id, This, type, 0, 0);
}

static string c2_classes[] = {
	"void", "bool",
	"int", "short", "char",
	"uint", "ushort", "uchar",
};

static rulei c2_grammar[] = {
	{"id", {}, identifier},
	{"number", {}, number},
	{"string", {}, literal},
	{"global", {"^?%import", "^?%enum", "^?%member_function", "^?%member_variable"}},
	{"import", {"import", "%url", "?%pseudoname", ";"}, add_type},
	{"pseudoname", {"as", "%id"}},
	{"url", {"%id", ". ?%.id"}, set_url},
	{"type", {"%id", "?.*"}, test_type},
	{"enum", {"enum", "{", "%enum_value", ", ?.%enum_value", "}", ";"}},
	{"enum_value", {"%id", "?%enum_assign"}},
	{"enum_assign", {"=", "%constant"}},
	{"constant", {"%expression"}, test_constant},
	{"initialization", {"=", "%expression"}},
	{"static", {"static"}, apply_static},
	{"public", {"public"}, apply_public},
	{"member", {"%type", "%id"}, set_member},
	{"parameter", {"%type", "%id"}},
	{"declare_function", {"?%static", "?%public", "%member", "(", "?%parameter", ", .?%parameter", ")"}, add_function},
	{"declare_variable", {"?%static", "?%public", "%member", "?%array_scope"}, add_variable},
	{"member_function", {"%declare_function", "%block_statements"}},
	{"member_variable", {"%declare_variable", "?%initialization", ";"}},
	{"local_variable", {"?%static", "%member", "?%array_scope", "?%initialization"}},
	{"sizeof", {"sizeof", "(", "%expression", ")"}},
	{"array_scope", {"[", "%expression", "]"}},
	{"expression", {"^?%number", "^?%string", "^?sizeof"}},
	{"block_statements", {"{", "?%single_statement", ".?%single_statement", "}"}},
	{"single_statement", {"?%statement", ";"}},
	{"statement", {"^?%local_variable"}},
	{"unary", {"^?%number", "^?%string", "^?sizeof"}},
	{"addiction_op", {"^?+", "^?-"}},
	{"addiction", {"%unary", "%addiction_op", "%unary"}},
	{"multiplication_op", {"^?/", "^?*", "^?%"}},
	{"multiplication", {"%addiction", "%multiplication_op", "%addiction"}},
};

BSDATA(lexer) = {
	{"C2", "*.c2", c2_grammar, c2_classes},
	{"Rust", "*.rs"}
};
BSDATAF(lexer)
