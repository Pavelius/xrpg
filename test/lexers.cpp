#include "codescan.h"

using namespace codescan;

static lexer::word c2_keywords[] = {
	{"auto"},
	{"import"},
	{"if"},
	{"for"},
	{"while"},
};
static lexer::word c2_types[] = {
	{"char"},
	{"int"},
	{"short"},
	{"bool"},
	{"this"},
	{"void"},
};
static lexer::word rust_keywords[] = {
	{"fn"},
	{"mod"},
	{"mut"},
	{"for"},
	{"in"},
	{"if"},
	{"else"},
	{"return"},
	{"while"},
	{"impl"},
	{"struct"},
	{"let"},
};
static lexer::word rust_types[] = {
	{"i64"},
	{"i32"},
	{"i16"},
	{"i8"},
	{"u64"},
	{"u32"},
	{"u16"},
	{"u8"},
	{"f64"},
	{"bool"},
	{"self"},
};
static lexer::word rust_constants[] = {
	{"true"},
	{"false"},
};

BSDATA(lexer) = {
	{"C2", "*.c2", c2_keywords, c2_types, rust_constants},
	{"Rust", "*.rs", rust_keywords, rust_types, rust_constants}
};
BSDATAF(lexer)
