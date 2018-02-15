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
 * Definitions for program data.
 *
 */

#ifndef BC_DATA_H
#define BC_DATA_H

#include <stdbool.h>

#include <bc.h>
#include <vector.h>
#include <num.h>

#define BC_PROGRAM_MAX_STMTS (128)

#define BC_PROGRAM_DEF_SIZE (16)

typedef enum BcTempType {

  BC_TEMP_NUM,
  BC_TEMP_NAME,

  BC_TEMP_SCALE,
  BC_TEMP_IBASE,
  BC_TEMP_OBASE,
  BC_TEMP_LAST,

} BcTempType;

typedef enum BcExprType {

  BC_EXPR_INC_PRE,
  BC_EXPR_DEC_PRE,

  BC_EXPR_INC_POST,
  BC_EXPR_DEC_POST,

  BC_EXPR_NEGATE,

  BC_EXPR_POWER,

  BC_EXPR_MULTIPLY,
  BC_EXPR_DIVIDE,
  BC_EXPR_MODULUS,

  BC_EXPR_PLUS,
  BC_EXPR_MINUS,

  BC_EXPR_ASSIGN_POWER,
  BC_EXPR_ASSIGN_MULTIPLY,
  BC_EXPR_ASSIGN_DIVIDE,
  BC_EXPR_ASSIGN_MODULUS,
  BC_EXPR_ASSIGN_PLUS,
  BC_EXPR_ASSIGN_MINUS,
  BC_EXPR_ASSIGN,

  BC_EXPR_REL_EQUAL,
  BC_EXPR_REL_LESS_EQ,
  BC_EXPR_REL_GREATER_EQ,
  BC_EXPR_REL_NOT_EQ,
  BC_EXPR_REL_LESS,
  BC_EXPR_REL_GREATER,

  BC_EXPR_BOOL_NOT,

  BC_EXPR_BOOL_OR,
  BC_EXPR_BOOL_AND,

  BC_EXPR_NUMBER,
  BC_EXPR_VAR,
  BC_EXPR_ARRAY_ELEM,

  BC_EXPR_FUNC_CALL,

  BC_EXPR_SCALE_FUNC,
  BC_EXPR_SCALE,
  BC_EXPR_IBASE,
  BC_EXPR_OBASE,
  BC_EXPR_LAST,
  BC_EXPR_LENGTH,
  BC_EXPR_READ,
  BC_EXPR_SQRT,

  BC_EXPR_PRINT,

} BcExprType;

typedef struct BcEntry {

  char* name;
  size_t idx;

} BcEntry;

typedef struct BcAuto {

  char* name;
  bool var;

} BcAuto;

typedef struct BcLocal {

  const char* name;
  bool var;

  union {
    BcNum num;
    struct {
      BcNum* array;
      uint32_t num_elems;
    };
  };

} BcLocal;

typedef struct BcTemp {

  BcTempType type;

  union {

    BcNum num;
    const char* name;

  };

} BcTemp;

typedef struct BcFunc {

  BcVec code;

  BcVec labels;

  BcAuto* params;
  uint32_t num_params;
  uint32_t param_cap;

  BcAuto* autos;
  uint32_t num_autos;
  uint32_t auto_cap;

} BcFunc;

typedef BcNum BcVar;

typedef BcVec BcArray;

typedef enum BcResultType {

  BC_NUM_RESULT,

  BC_NUM_CONSTANT,

  BC_NUM_VAR,
  BC_NUM_ARRAY,

  BC_NUM_SCALE,
  BC_NUM_LAST,
  BC_NUM_IBASE,
  BC_NUM_OBASE,

} BcResultType;

typedef struct BcResult {

  BcResultType type;
  BcNum num;

} BcResult;

typedef struct BcInstPtr {

  size_t func;
  size_t idx;

} BcInstPtr;

BcStatus bc_func_init(BcFunc* func);
BcStatus bc_func_insertParam(BcFunc* func, char* name, bool var);
BcStatus bc_func_insertAuto(BcFunc* func, char* name, bool var);
void bc_func_free(void* func);

BcStatus bc_var_init(BcVar* var);
void bc_var_free(void* var);

BcStatus bc_array_init(BcArray* array);
void bc_array_free(void* array);

BcStatus bc_local_initVar(BcLocal* local, const char* name, const char* num);
BcStatus bc_local_initArray(BcLocal* local, const char* name, uint32_t nelems);
void bc_local_free(void* local);

BcStatus bc_temp_initNum(BcTemp* temp, const char* val);
BcStatus bc_temp_initName(BcTemp* temp, const char* name);
BcStatus bc_temp_init(BcTemp* temp, BcTempType type);
void bc_temp_free(void* temp);

void bc_string_free(void* string);

int bc_entry_cmp(void* entry1, void*entry2);
void bc_entry_free(void* entry);

void bc_result_free(void* num);

void bc_constant_free(void* constant);

#endif // BC_DATA_H