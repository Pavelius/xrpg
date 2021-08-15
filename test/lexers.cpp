#include "code.h"

using namespace code;

static word c2_keywords[] = {
	{"auto"},
	{"import"},
	{"if"},
	{"for"},
	{"while"},
};
static word c2_types[] = {
	{"char"},
	{"int"},
	{"short"},
	{"bool"},
	{"this"},
	{"void"},
};
static word c2_operators[] = {
	{"++"},
	{"--"},
	{"!="},
	{"=="},
	{"+="},
	{"-="},
	{"/="},
	{"*="},
	{"%="},
	{"|="},
	{"^="},
	{"&="},
	{"&&"},
	{"||"},
};
static word rust_keywords[] = {
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
static word rust_types[] = {
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
static word rust_constants[] = {
	{"true"},
	{"false"},
};

BSDATA(lexer) = {
	{"C2", "*.c2", c2_keywords, c2_types, rust_constants, c2_operators},
	{"Rust", "*.rs", rust_keywords, rust_types, rust_constants}
};
BSDATAF(lexer)
