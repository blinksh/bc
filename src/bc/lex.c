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
 * The lexer for bc.
 *
 */

#include <assert.h>
#include <ctype.h>
#include <string.h>

#include <lex.h>
#include <bc.h>
#include <vm.h>
#include <TargetConditionals.h>
#ifdef TARGET_OS_IPHONE
#include "ios_error.h"
#endif

#ifdef BC_ENABLED
BcStatus bc_lex_identifier(BcLex *l) {

	BcStatus s;
	size_t i;
	const char *buf = l->buf + l->i - 1;

	for (i = 0; i < sizeof(bc_lex_kws) / sizeof(bc_lex_kws[0]); ++i) {

		unsigned long len = (unsigned long) bc_lex_kws[i].len;

		if (strncmp(buf, bc_lex_kws[i].name, len) == 0) {

			l->t.t = BC_LEX_KEY_AUTO + (BcLexType) i;

			if (!bc_lex_kws[i].posix) {
				s = bc_vm_posixError(BC_STATUS_POSIX_BAD_KW, l->f,
				                     l->line, bc_lex_kws[i].name);
				if (s) return s;
			}

			// We minus 1 because the index has already been incremented.
			l->i += len - 1;
			return BC_STATUS_SUCCESS;
		}
	}

	s = bc_lex_name(l);
	if (s) return s;

	if (l->t.v.len - 1 > 1)
		s = bc_vm_posixError(BC_STATUS_POSIX_NAME_LEN, l->f, l->line, buf);

	return s;
}

BcStatus bc_lex_string(BcLex *l) {

	size_t len, nls = 0, i = l->i;
	char c;

	l->t.t = BC_LEX_STR;

	for (c = l->buf[i]; c != 0 && c != '"'; c = l->buf[++i]) nls += (c == '\n');

	if (c == '\0') {
		l->i = i;
		return BC_STATUS_LEX_NO_STRING_END;
	}

	len = i - l->i;
	if (len > BC_MAX_STRING) return BC_STATUS_EXEC_STRING_LEN;
	bc_vec_string(&l->t.v, len, l->buf + l->i);

	l->i = i + 1;
	l->line += nls;

	return BC_STATUS_SUCCESS;
}

void bc_lex_assign(BcLex *l, BcLexType with, BcLexType without) {
	if (l->buf[l->i] == '=') {
		++l->i;
		l->t.t = with;
	}
	else l->t.t = without;
}

BcStatus bc_lex_comment(BcLex *l) {

	size_t i, nls = 0;
	const char *buf = l->buf;
	bool end = false;
	char c;

	l->t.t = BC_LEX_WHITESPACE;

	for (i = ++l->i; !end; i += !end) {

		for (c = buf[i]; c != '*' && c != 0; c = buf[++i]) nls += (c == '\n');

		if (c == 0 || buf[i + 1] == '\0') {
			l->i = i;
			return BC_STATUS_LEX_NO_COMMENT_END;
		}

		end = buf[i + 1] == '/';
	}

	l->i = i + 2;
	l->line += nls;

	return BC_STATUS_SUCCESS;
}

BcStatus bc_lex_token(BcLex *l) {

	BcStatus s = BC_STATUS_SUCCESS;
	char c = l->buf[l->i++], c2;

	// This is the workhorse of the lexer.
	switch (c) {

		case '\0':
		case '\n':
		{
			l->newline = true;
			l->t.t = !c ? BC_LEX_EOF : BC_LEX_NLINE;
			break;
		}

		case '\t':
		case '\v':
		case '\f':
		case '\r':
		case ' ':
		{
			bc_lex_whitespace(l);
			break;
		}

		case '!':
		{
			bc_lex_assign(l, BC_LEX_OP_REL_NE, BC_LEX_OP_BOOL_NOT);

			if (l->t.t == BC_LEX_OP_BOOL_NOT) {
				s = bc_vm_posixError(BC_STATUS_POSIX_BOOL, l->f, l->line, "!");
				if (s) return s;
			}

			break;
		}

		case '"':
		{
			s = bc_lex_string(l);
			break;
		}

		case '#':
		{
			s = bc_vm_posixError(BC_STATUS_POSIX_COMMENT, l->f, l->line, NULL);
			if (s) return s;

			bc_lex_lineComment(l);

			break;
		}

		case '%':
		{
			bc_lex_assign(l, BC_LEX_OP_ASSIGN_MODULUS, BC_LEX_OP_MODULUS);
			break;
		}

		case '&':
		{
			c2 = l->buf[l->i];
			if (c2 == '&') {

				s = bc_vm_posixError(BC_STATUS_POSIX_BOOL, l->f, l->line, "&&");
				if (s) return s;

				++l->i;
				l->t.t = BC_LEX_OP_BOOL_AND;
			}
			else {
				l->t.t = BC_LEX_INVALID;
				s = BC_STATUS_LEX_BAD_CHAR;
			}

			break;
		}

		case '(':
		case ')':
		{
			l->t.t = (BcLexType) (c - '(' + BC_LEX_LPAREN);
			break;
		}

		case '*':
		{
			bc_lex_assign(l, BC_LEX_OP_ASSIGN_MULTIPLY, BC_LEX_OP_MULTIPLY);
			break;
		}

		case '+':
		{
			c2 = l->buf[l->i];
			if (c2 == '+') {
				++l->i;
				l->t.t = BC_LEX_OP_INC;
			}
			else bc_lex_assign(l, BC_LEX_OP_ASSIGN_PLUS, BC_LEX_OP_PLUS);
			break;
		}

		case ',':
		{
			l->t.t = BC_LEX_COMMA;
			break;
		}

		case '-':
		{
			c2 = l->buf[l->i];
			if (c2 == '-') {
				++l->i;
				l->t.t = BC_LEX_OP_DEC;
			}
			else bc_lex_assign(l, BC_LEX_OP_ASSIGN_MINUS, BC_LEX_OP_MINUS);
			break;
		}

		case '.':
		{
			if (isdigit(l->buf[l->i])) s = bc_lex_number(l, c);
			else {
				l->t.t = BC_LEX_KEY_LAST;
				s = bc_vm_posixError(BC_STATUS_POSIX_DOT, l->f, l->line, NULL);
			}
			break;
		}

		case '/':
		{
			c2 = l->buf[l->i];
			if (c2 =='*') s = bc_lex_comment(l);
			else bc_lex_assign(l, BC_LEX_OP_ASSIGN_DIVIDE, BC_LEX_OP_DIVIDE);
			break;
		}

		case '0':
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
		case '8':
		case '9':
		case 'A':
		case 'B':
		case 'C':
		case 'D':
		case 'E':
		case 'F':
		{
			s = bc_lex_number(l, c);
			break;
		}

		case ';':
		{
			l->t.t = BC_LEX_SCOLON;
			break;
		}

		case '<':
		{
			bc_lex_assign(l, BC_LEX_OP_REL_LE, BC_LEX_OP_REL_LT);
			break;
		}

		case '=':
		{
			bc_lex_assign(l, BC_LEX_OP_REL_EQ, BC_LEX_OP_ASSIGN);
			break;
		}

		case '>':
		{
			bc_lex_assign(l, BC_LEX_OP_REL_GE, BC_LEX_OP_REL_GT);
			break;
		}

		case '[':
		case ']':
		{
			l->t.t = (BcLexType) (c - '[' + BC_LEX_LBRACKET);
			break;
		}

		case '\\':
		{
			if (l->buf[l->i] == '\n') {
				l->t.t = BC_LEX_WHITESPACE;
				++l->i;
			}
			else s = BC_STATUS_LEX_BAD_CHAR;
			break;
		}

		case '^':
		{
			bc_lex_assign(l, BC_LEX_OP_ASSIGN_POWER, BC_LEX_OP_POWER);
			break;
		}

		case 'a':
		case 'b':
		case 'c':
		case 'd':
		case 'e':
		case 'f':
		case 'g':
		case 'h':
		case 'i':
		case 'j':
		case 'k':
		case 'l':
		case 'm':
		case 'n':
		case 'o':
		case 'p':
		case 'q':
		case 'r':
		case 's':
		case 't':
		case 'u':
		case 'v':
		case 'w':
		case 'x':
		case 'y':
		case 'z':
		{
			s = bc_lex_identifier(l);
			break;
		}

		case '{':
		case '}':
		{
			l->t.t = (BcLexType) (c - '{' + BC_LEX_LBRACE);
			break;
		}

		case '|':
		{
			c2 = l->buf[l->i];

			if (c2 == '|') {

				s = bc_vm_posixError(BC_STATUS_POSIX_BOOL, l->f, l->line, "||");
				if (s) return s;

				++l->i;
				l->t.t = BC_LEX_OP_BOOL_OR;
			}
			else {
				l->t.t = BC_LEX_INVALID;
				s = BC_STATUS_LEX_BAD_CHAR;
			}

			break;
		}

		default:
		{
			l->t.t = BC_LEX_INVALID;
			s = BC_STATUS_LEX_BAD_CHAR;
			break;
		}
	}

	return s;
}
#endif // BC_ENABLED
