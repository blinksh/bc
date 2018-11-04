/*
 * *****************************************************************************
 *
 * Copyright 2018 Gavin D. Howard
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH
 * REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY
 * AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT,
 * INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM
 * LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR
 * OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 *
 * *****************************************************************************
 *
 * Constant data for bc.
 *
 */

#include <lex.h>
#include <parse.h>
#include <bc.h>
#include <num.h>
#include <TargetConditionals.h>
#ifdef TARGET_OS_IPHONE
#include "ios_error.h"
#endif

// clang-format off

#ifdef BC_ENABLED
// ** Exclude start. **
const char bc_name[] = "bc";
// ** Exclude end. **
#if BC_ENABLE_SIGNALS
const char bc_sig_msg[] = "\ninterrupt (type \"quit\" to exit)\n";
#endif // BC_ENABLE_SIGNALS
#endif // BC_ENABLED

#ifdef DC_ENABLED
const char dc_name[] = "dc";
#if BC_ENABLE_SIGNALS
const char dc_sig_msg[] = "\ninterrupt (type \"q\" to exit)\n";
#endif // BC_ENABLE_SIGNALS
#endif // DC_ENABLED

const char bc_copyright[] =
	"Copyright (c) 2018 Gavin D. Howard and contributors\n"
	"Report bugs at: https://github.com/gavinhoward/bc\n\n"
	"This is free software with ABSOLUTELY NO WARRANTY.\n";

// ** Exclude start. **
const char* const bc_args_env_name = "BC_ENV_ARGS";
// ** Exclude end. **

const char bc_err_fmt[] = "\n%s error: %s\n";
const char bc_warn_fmt[] = "\n%s warning: %s\n";
const char bc_err_line[] = ":%zu\n\n";

const char *bc_errs[] = {
	"VM",
	"Lex",
	"Parse",
	"Math",
	"Runtime",
	"Vector",
#ifdef BC_ENABLED
	"POSIX",
#endif // BC_ENABLED
};

const uint8_t bc_err_ids[] = {
	BC_ERR_IDX_VM, BC_ERR_IDX_VM, BC_ERR_IDX_VM, BC_ERR_IDX_VM, BC_ERR_IDX_VM,
	BC_ERR_IDX_LEX, BC_ERR_IDX_LEX, BC_ERR_IDX_LEX, BC_ERR_IDX_LEX,
#ifdef DC_ENABLED
	BC_ERR_IDX_LEX,
#endif // DC_ENABLED
	BC_ERR_IDX_PARSE, BC_ERR_IDX_PARSE, BC_ERR_IDX_PARSE, BC_ERR_IDX_PARSE,
	BC_ERR_IDX_PARSE, BC_ERR_IDX_PARSE, BC_ERR_IDX_PARSE, BC_ERR_IDX_PARSE,
	BC_ERR_IDX_MATH, BC_ERR_IDX_MATH, BC_ERR_IDX_MATH, BC_ERR_IDX_MATH,
	BC_ERR_IDX_MATH,
#ifdef DC_ENABLED
	BC_ERR_IDX_MATH,
#endif // DC_ENABLED
	BC_ERR_IDX_EXEC, BC_ERR_IDX_EXEC, BC_ERR_IDX_EXEC, BC_ERR_IDX_EXEC,
	BC_ERR_IDX_EXEC, BC_ERR_IDX_EXEC, BC_ERR_IDX_EXEC, BC_ERR_IDX_EXEC,
	BC_ERR_IDX_EXEC, BC_ERR_IDX_EXEC, BC_ERR_IDX_EXEC, BC_ERR_IDX_EXEC,
	BC_ERR_IDX_EXEC, BC_ERR_IDX_EXEC, BC_ERR_IDX_EXEC, BC_ERR_IDX_EXEC,
	BC_ERR_IDX_EXEC,
	BC_ERR_IDX_VEC, BC_ERR_IDX_VEC,
#ifdef BC_ENABLED
	BC_ERR_IDX_POSIX, BC_ERR_IDX_POSIX, BC_ERR_IDX_POSIX, BC_ERR_IDX_POSIX,
	BC_ERR_IDX_POSIX, BC_ERR_IDX_POSIX, BC_ERR_IDX_POSIX, BC_ERR_IDX_POSIX,
	BC_ERR_IDX_POSIX, BC_ERR_IDX_POSIX, BC_ERR_IDX_POSIX, BC_ERR_IDX_POSIX,
#endif // BC_ENABLED
	BC_ERR_IDX_VM, BC_ERR_IDX_VM, BC_ERR_IDX_VM,
};

const char *bc_err_msgs[] = {

	NULL,
	"memory allocation error",
	"I/O error",
	"file is not text:",
	"path is a directory:",

	"bad character",
	"string end could not be found",
	"comment end could not be found",
	"end of file",
#ifdef DC_ENABLED
	"extended register",
#endif // DC_ENABLED

	"bad token",
	"bad expression",
	"empty expression",
	"bad print statement",
	"bad function definition",
	"bad assignment: left side must be scale, ibase, "
		"obase, last, var, or array element",
	"no auto variable found",
	"function parameter or auto var has the same name as another",
	"block end could not be found",

	"negative number",
	"non integer number",
	"overflow",
	"divide by zero",
	"bad number string",

	"could not open file:",
	"mismatched parameters",
	"undefined function",
	"file is not executable:",
	"number too long: must be [1, BC_NUM_MAX]",
	"name too long: must be [1, BC_NAME_MAX]",
	"string too long: must be [1, BC_STRING_MAX]",
	"array too long; must be [1, BC_DIM_MAX]",
	"bad ibase; must be [2, 16]",
	"bad scale; must be [0, BC_SCALE_MAX]",
	"bad read() expression",
	"read() call inside of a read() call",
	"variable is wrong type",
	"bad obase; must be [2, BC_BASE_MAX]",
	"signal caught and not handled",
	"stack has too few elements",

	"index is out of bounds",
	"item already exists",

#ifdef BC_ENABLED
	"POSIX only allows one character names; the following is bad:",
	"POSIX does not allow '#' script comments",
	"POSIX does not allow the following keyword:",
	"POSIX does not allow a period ('.') as a shortcut for the last result",
	"POSIX requires parentheses around return expressions",
	"POSIX does not allow boolean operators; the following is bad:",
	"POSIX does not allow comparison operators outside if or loops",
	"POSIX requires exactly one comparison operator per condition",
	"POSIX does not allow an empty init expression in a for loop",
	"POSIX does not allow an empty condition expression in a for loop",
	"POSIX does not allow an empty update expression in a for loop",
	"POSIX requires the left brace be on the same line as the function header",
#endif // BC_ENABLED

#ifndef NDEBUG
	"quit request not honored",
	"limits request not honored",
#endif // NDEBUG

};

const char bc_func_main[] = "(main)";
const char bc_func_read[] = "(read)";

#ifndef NDEBUG
const char bc_inst_chars[] =
	"edED_^*/%+-=;?~<>!|&`{}@[],NVMACaI.LlrOqpQsSJjPR$bHovxzatfcZdghkKuU";
#endif // NDEBUG

#ifdef BC_ENABLED
const BcLexKeyword bc_lex_kws[20] = {
	BC_LEX_KW_ENTRY("auto", 4, true),
	BC_LEX_KW_ENTRY("break", 5, true),
	BC_LEX_KW_ENTRY("continue", 8, false),
	BC_LEX_KW_ENTRY("define", 6, true),
	BC_LEX_KW_ENTRY("else", 4, false),
	BC_LEX_KW_ENTRY("for", 3, true),
	BC_LEX_KW_ENTRY("halt", 4, false),
	BC_LEX_KW_ENTRY("ibase", 5, true),
	BC_LEX_KW_ENTRY("if", 2, true),
	BC_LEX_KW_ENTRY("last", 4, false),
	BC_LEX_KW_ENTRY("length", 6, true),
	BC_LEX_KW_ENTRY("limits", 6, false),
	BC_LEX_KW_ENTRY("obase", 5, true),
	BC_LEX_KW_ENTRY("print", 5, false),
	BC_LEX_KW_ENTRY("quit", 4, true),
	BC_LEX_KW_ENTRY("read", 4, false),
	BC_LEX_KW_ENTRY("return", 6, true),
	BC_LEX_KW_ENTRY("scale", 5, true),
	BC_LEX_KW_ENTRY("sqrt", 4, true),
	BC_LEX_KW_ENTRY("while", 5, true),
};

// This is an array that corresponds to token types. An entry is
// true if the token is valid in an expression, false otherwise.
const bool bc_parse_exprs[] = {
	false, false, true, true, true, true, true, true, true, true, true, true,
	true, true, true, true, true, true, true, true, true, true, true, true,
	true, true, true, false, false, true, true, false, false, false, false,
	false, false, false, true, true, false, false, false, false, false, false,
	false, true, false, true, true, true, true, false, false, true, false, true,
	true, false,
};

// This is an array of data for operators that correspond to token types.
const BcOp bc_parse_ops[] = {
	{ 0, false }, { 0, false },
	{ 1, false },
	{ 2, false },
	{ 3, true }, { 3, true }, { 3, true },
	{ 4, true }, { 4, true },
	{ 6, true }, { 6, true }, { 6, true }, { 6, true }, { 6, true }, { 6, true },
	{ 1, false },
	{ 7, true }, { 7, true },
	{ 5, false }, { 5, false }, { 5, false }, { 5, false }, { 5, false },
	{ 5, false }, { 5, false },
};

// These identify what tokens can come after expressions in certain cases.
const BcParseNext bc_parse_next_expr =
	BC_PARSE_NEXT(4, BC_LEX_NLINE, BC_LEX_SCOLON, BC_LEX_RBRACE, BC_LEX_EOF);
const BcParseNext bc_parse_next_param =
	BC_PARSE_NEXT(2, BC_LEX_RPAREN, BC_LEX_COMMA);
const BcParseNext bc_parse_next_print =
	BC_PARSE_NEXT(4, BC_LEX_COMMA, BC_LEX_NLINE, BC_LEX_SCOLON, BC_LEX_EOF);
const BcParseNext bc_parse_next_rel = BC_PARSE_NEXT(1, BC_LEX_RPAREN);
const BcParseNext bc_parse_next_elem = BC_PARSE_NEXT(1, BC_LEX_RBRACKET);
const BcParseNext bc_parse_next_for = BC_PARSE_NEXT(1, BC_LEX_SCOLON);
const BcParseNext bc_parse_next_read =
	BC_PARSE_NEXT(2, BC_LEX_NLINE, BC_LEX_EOF);
#endif // BC_ENABLED

#ifdef DC_ENABLED
const BcLexType dc_lex_regs[] = {
	BC_LEX_OP_REL_EQ, BC_LEX_OP_REL_LE, BC_LEX_OP_REL_GE, BC_LEX_OP_REL_NE,
	BC_LEX_OP_REL_LT, BC_LEX_OP_REL_GT, BC_LEX_SCOLON, BC_LEX_COLON,
	BC_LEX_ELSE, BC_LEX_LOAD, BC_LEX_LOAD_POP, BC_LEX_OP_ASSIGN,
	BC_LEX_STORE_PUSH,
};

const size_t dc_lex_regs_len = sizeof(dc_lex_regs) / sizeof(BcLexType);

const BcLexType dc_lex_tokens[] = {
	BC_LEX_OP_MODULUS, BC_LEX_INVALID, BC_LEX_INVALID, BC_LEX_LPAREN,
	BC_LEX_INVALID, BC_LEX_OP_MULTIPLY, BC_LEX_OP_PLUS, BC_LEX_INVALID,
	BC_LEX_OP_MINUS, BC_LEX_INVALID, BC_LEX_OP_DIVIDE,
	BC_LEX_INVALID, BC_LEX_INVALID, BC_LEX_INVALID, BC_LEX_INVALID,
	BC_LEX_INVALID, BC_LEX_INVALID, BC_LEX_INVALID, BC_LEX_INVALID,
	BC_LEX_INVALID, BC_LEX_INVALID,
	BC_LEX_COLON, BC_LEX_SCOLON, BC_LEX_OP_REL_GT, BC_LEX_OP_REL_EQ,
	BC_LEX_OP_REL_LT, BC_LEX_KEY_READ, BC_LEX_INVALID,
	BC_LEX_INVALID, BC_LEX_INVALID, BC_LEX_INVALID, BC_LEX_INVALID,
	BC_LEX_INVALID, BC_LEX_INVALID, BC_LEX_EQ_NO_REG, BC_LEX_INVALID,
	BC_LEX_KEY_IBASE, BC_LEX_INVALID, BC_LEX_KEY_SCALE, BC_LEX_LOAD_POP,
	BC_LEX_INVALID, BC_LEX_OP_BOOL_NOT, BC_LEX_KEY_OBASE, BC_LEX_PRINT_STREAM,
	BC_LEX_NQUIT, BC_LEX_POP, BC_LEX_STORE_PUSH, BC_LEX_INVALID, BC_LEX_INVALID,
	BC_LEX_INVALID, BC_LEX_INVALID, BC_LEX_SCALE_FACTOR, BC_LEX_INVALID,
	BC_LEX_KEY_LENGTH, BC_LEX_INVALID, BC_LEX_INVALID, BC_LEX_INVALID,
	BC_LEX_OP_POWER, BC_LEX_NEG, BC_LEX_INVALID,
	BC_LEX_ASCIIFY, BC_LEX_INVALID, BC_LEX_CLEAR_STACK, BC_LEX_DUPLICATE,
	BC_LEX_ELSE, BC_LEX_PRINT_STACK, BC_LEX_INVALID, BC_LEX_INVALID,
	BC_LEX_STORE_IBASE, BC_LEX_INVALID, BC_LEX_STORE_SCALE, BC_LEX_LOAD,
	BC_LEX_INVALID, BC_LEX_PRINT_POP, BC_LEX_STORE_OBASE, BC_LEX_KEY_PRINT,
	BC_LEX_KEY_QUIT, BC_LEX_SWAP, BC_LEX_OP_ASSIGN, BC_LEX_INVALID,
	BC_LEX_INVALID, BC_LEX_KEY_SQRT, BC_LEX_INVALID, BC_LEX_EXECUTE,
	BC_LEX_INVALID, BC_LEX_STACK_LEVEL,
	BC_LEX_LBRACE, BC_LEX_OP_MODEXP, BC_LEX_INVALID, BC_LEX_OP_DIVMOD,
	BC_LEX_INVALID
};

const BcInst dc_parse_insts[] = {
	BC_INST_INVALID, BC_INST_INVALID, BC_INST_INVALID, BC_INST_REL_GE,
	BC_INST_INVALID, BC_INST_POWER, BC_INST_MULTIPLY, BC_INST_DIVIDE,
	BC_INST_MODULUS, BC_INST_PLUS, BC_INST_MINUS,
	BC_INST_INVALID, BC_INST_INVALID, BC_INST_INVALID, BC_INST_INVALID,
	BC_INST_INVALID, BC_INST_INVALID,
	BC_INST_BOOL_NOT, BC_INST_INVALID, BC_INST_INVALID,
	BC_INST_INVALID, BC_INST_INVALID, BC_INST_INVALID, BC_INST_INVALID,
	BC_INST_INVALID, BC_INST_INVALID, BC_INST_INVALID,
	BC_INST_INVALID, BC_INST_INVALID, BC_INST_REL_GT, BC_INST_INVALID,
	BC_INST_INVALID, BC_INST_INVALID, BC_INST_INVALID, BC_INST_REL_GE,
	BC_INST_INVALID, BC_INST_INVALID,
	BC_INST_INVALID, BC_INST_INVALID, BC_INST_INVALID,
	BC_INST_INVALID, BC_INST_INVALID, BC_INST_INVALID, BC_INST_INVALID,
	BC_INST_INVALID, BC_INST_INVALID, BC_INST_INVALID, BC_INST_IBASE,
	BC_INST_INVALID, BC_INST_INVALID, BC_INST_LENGTH, BC_INST_INVALID,
	BC_INST_OBASE, BC_INST_PRINT, BC_INST_QUIT, BC_INST_INVALID,
	BC_INST_INVALID, BC_INST_SCALE, BC_INST_SQRT, BC_INST_INVALID,
	BC_INST_REL_EQ, BC_INST_MODEXP, BC_INST_DIVMOD, BC_INST_INVALID,
	BC_INST_INVALID, BC_INST_EXECUTE, BC_INST_PRINT_STACK, BC_INST_CLEAR_STACK,
	BC_INST_STACK_LEN, BC_INST_DUPLICATE, BC_INST_SWAP, BC_INST_POP,
	BC_INST_ASCIIFY, BC_INST_PRINT_STREAM, BC_INST_INVALID, BC_INST_INVALID,
	BC_INST_INVALID, BC_INST_INVALID, BC_INST_INVALID, BC_INST_INVALID,
	BC_INST_PRINT, BC_INST_NQUIT, BC_INST_SCALE_FUNC,
};
#endif // DC_ENABLED

const char bc_num_hex_digits[] = "0123456789ABCDEF";

const BcNumBinaryOp bc_program_ops[] = {
	bc_num_pow, bc_num_mul, bc_num_div, bc_num_mod, bc_num_add, bc_num_sub,
};

// ** Exclude start. **
// ** Busybox exclude start. **
const char bc_program_exprs_name[] = "<exprs>";
// ** Busybox exclude end. **
// ** Exclude end. **

const char bc_program_stdin_name[] = "<stdin>";
const char bc_program_ready_msg[] = "ready for more input\n";

// clang-format on
