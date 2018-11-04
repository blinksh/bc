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
 * Code common to the parsers.
 *
 */

#include <assert.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include <limits.h>

#include <status.h>
#include <vector.h>
#include <lex.h>
#include <parse.h>
#include <program.h>
#include <vm.h>
#include <TargetConditionals.h>
#ifdef TARGET_OS_IPHONE
#include "ios_error.h"
#endif

void bc_parse_addFunc(BcParse *p, char *name, size_t *idx) {
	bc_program_addFunc(p->prog, name, idx);
	p->func = bc_vec_item(&p->prog->fns, p->fidx);
}

void bc_parse_pushName(BcParse *p, char *name) {

	size_t i = 0, len = strlen(name);

	for (; i < len; ++i) bc_parse_push(p, name[i]);
	bc_parse_push(p, BC_PARSE_STREND);

	free(name);
}

void bc_parse_pushIndex(BcParse *p, size_t idx) {

	unsigned char amt, i, nums[sizeof(size_t)];

	for (amt = 0; idx; ++amt) {
		nums[amt] = (char) idx;
		idx = (idx & ((unsigned long) ~(UCHAR_MAX))) >> sizeof(char) * CHAR_BIT;
	}

	bc_parse_push(p, amt);
	for (i = 0; i < amt; ++i) bc_parse_push(p, nums[i]);
}

void bc_parse_number(BcParse *p, BcInst *prev, size_t *nexs) {

	char *num = bc_vm_strdup(p->l.t.v.v);
	size_t idx = p->prog->consts.len;

	bc_vec_push(&p->prog->consts, &num);

	bc_parse_push(p, BC_INST_NUM);
	bc_parse_pushIndex(p, idx);

	++(*nexs);
	(*prev) = BC_INST_NUM;
}

BcStatus bc_parse_text(BcParse *p, const char *text) {

	BcStatus s;

	p->func = bc_vec_item(&p->prog->fns, p->fidx);

	if (!strcmp(text, "") && !BC_PARSE_CAN_EXEC(p)) {
		p->l.t.t = BC_LEX_INVALID;
		s = p->parse(p);
		if (s) return s;
		if (!BC_PARSE_CAN_EXEC(p)) return BC_STATUS_EXEC_FILE_NOT_EXECUTABLE;
	}

	return bc_lex_text(&p->l, text);
}

BcStatus bc_parse_reset(BcParse *p, BcStatus s) {

	if (p->fidx != BC_PROG_MAIN) {

		p->func->nparams = 0;
		bc_vec_npop(&p->func->code, p->func->code.len);
		bc_vec_npop(&p->func->autos, p->func->autos.len);
		bc_vec_npop(&p->func->labels, p->func->labels.len);

		bc_parse_updateFunc(p, BC_PROG_MAIN);
	}

	p->l.i = p->l.len;
	p->l.t.t = BC_LEX_EOF;
	p->auto_part = (p->nbraces = 0);

	bc_vec_npop(&p->flags, p->flags.len - 1);
	bc_vec_npop(&p->exits, p->exits.len);
	bc_vec_npop(&p->conds, p->conds.len);
	bc_vec_npop(&p->ops, p->ops.len);

	return bc_program_reset(p->prog, s);
}

void bc_parse_free(BcParse *p) {
	assert(p);
	bc_vec_free(&p->flags);
	bc_vec_free(&p->exits);
	bc_vec_free(&p->conds);
	bc_vec_free(&p->ops);
	bc_lex_free(&p->l);
}

void bc_parse_create(BcParse *p, BcProgram *prog, size_t func,
                     BcParseParse parse, BcLexNext next)
{
	assert(p && prog);

	memset(p, 0, sizeof(BcParse));

	bc_lex_init(&p->l, next);
	bc_vec_init(&p->flags, sizeof(uint8_t), NULL);
	bc_vec_init(&p->exits, sizeof(BcInstPtr), NULL);
	bc_vec_init(&p->conds, sizeof(size_t), NULL);
	bc_vec_pushByte(&p->flags, 0);
	bc_vec_init(&p->ops, sizeof(BcLexType), NULL);

	p->parse = parse;
	p->prog = prog;
	p->auto_part = (p->nbraces = 0);
	bc_parse_updateFunc(p, func);
}
