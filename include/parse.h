﻿/*
 * Copyright 2018 Contributors
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.

 * 3. Neither the name of the copyright holder nor the names of its contributors
 * may be used to endorse or promote products derived from this software without
 * specific prior written permission.

 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 *******************************************************************************
 *
 * A special license exemption is granted to the Toybox project to use this
 * source under the following BSD 0-Clause License:
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
 *******************************************************************************
 *
 * Definitions for bc's parser.
 *
 */

#ifndef BC_PARSE_H
#define BC_PARSE_H

#include <stdbool.h>
#include <stdint.h>

#include <vector.h>
#include <program.h>
#include <lex.h>

#define BC_PARSE_TOP_FLAG_PTR(parse)  \
  ((uint8_t*) bc_vec_top(&(parse)->flags))

#define BC_PARSE_TOP_FLAG(parse)  \
  (*(BC_PARSE_TOP_FLAG_PTR(parse)))

#define BC_PARSE_FLAG_FUNC_INNER (0x01)

#define BC_PARSE_FUNC_INNER(parse)  \
  (BC_PARSE_TOP_FLAG(parse) & BC_PARSE_FLAG_FUNC_INNER)

#define BC_PARSE_FLAG_FUNC (0x02)

#define BC_PARSE_FUNC(parse)  \
  (BC_PARSE_TOP_FLAG(parse) & BC_PARSE_FLAG_FUNC)

#define BC_PARSE_FLAG_HEADER (0x04)

#define BC_PARSE_HEADER(parse)  \
  (BC_PARSE_TOP_FLAG(parse) & BC_PARSE_FLAG_HEADER)

#define BC_PARSE_FLAG_LOOP (0x08)

#define BC_PARSE_LOOP(parse)  \
  (BC_PARSE_TOP_FLAG(parse) & BC_PARSE_FLAG_LOOP)

#define BC_PARSE_FLAG_LOOP_INNER (0x10)

#define BC_PARSE_LOOP_INNER(parse) \
  (BC_PARSE_TOP_FLAG(parse) & BC_PARSE_FLAG_LOOP_INNER)

#define BC_PARSE_FLAG_IF (0x20)

#define BC_PARSE_IF(parse)  \
  (BC_PARSE_TOP_FLAG(parse) & BC_PARSE_FLAG_IF)

#define BC_PARSE_FLAG_ELSE (0x40)

#define BC_PARSE_ELSE(parse)  \
  (BC_PARSE_TOP_FLAG(parse) & BC_PARSE_FLAG_ELSE)

#define BC_PARSE_FLAG_IF_END (0x80)

#define BC_PARSE_IF_END(parse)  \
  (BC_PARSE_TOP_FLAG(parse) & BC_PARSE_FLAG_IF_END)

#define BC_PARSE_CAN_EXEC(parse)  \
  (!(BC_PARSE_TOP_FLAG(parse) & (BC_PARSE_FLAG_FUNC_INNER |  \
                                 BC_PARSE_FLAG_FUNC |        \
                                 BC_PARSE_FLAG_HEADER |      \
                                 BC_PARSE_FLAG_LOOP |        \
                                 BC_PARSE_FLAG_LOOP_INNER |  \
                                 BC_PARSE_FLAG_IF |          \
                                 BC_PARSE_FLAG_ELSE |        \
                                 BC_PARSE_FLAG_IF_END)))

// We can calculate the conversion between tokens and exprs
// by subtracting the position of the first operator in the
// lex enum and adding the position of the first in the expr
// enum. WARNING: This only works for binary operators.
#define BC_PARSE_TOKEN_TO_EXPR(type) ((type) - BC_LEX_OP_POWER + BC_EXPR_POWER)

typedef struct BcOp {

  uint8_t prec;
  bool left;

} BcOp;

typedef struct BcParse {

  BcLex lex;
  BcLexToken token;

  BcVec flags;

  BcVec exit_labels;

  BcVec cond_labels;

  BcVec ops;

  BcProgram* program;
  size_t func;

  uint32_t num_braces;

  bool auto_part;

} BcParse;

BcStatus bc_parse_init(BcParse* parse, BcProgram* program);
BcStatus bc_parse_file(BcParse* parse, const char* file);
BcStatus bc_parse_text(BcParse* parse, const char* text);

BcStatus bc_parse_parse(BcParse* parse);

BcStatus bc_parse_expr(BcParse* parse, BcVec* code, bool posix_rel, bool print);

void bc_parse_free(BcParse* parse);

#endif // BC_PARSE_H