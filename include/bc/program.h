#ifndef BC_PROGRAM_H
#define BC_PROGRAM_H

#include <stdbool.h>
#include <stdint.h>

#include <bc/bc.h>
#include <bc/data.h>

typedef struct BcProgram {

	BcStmtList* list;

	uint32_t stmt_idx;

	long scale;
	long ibase;
	long obase;

	long base_max;
	long dim_max;
	long scale_max;
	long string_max;

	BcStack ctx_stack;

	BcStack locals;

	BcStack temps;

	BcSegArray funcs;

	BcSegArray vars;

	BcSegArray arrays;

	const char* file;

} BcProgram;

BcStatus bc_program_init(BcProgram* p, const char* file);
void bc_program_limits(BcProgram* p);
BcStatus bc_program_func_add(BcProgram* p, BcFunc* func);
BcStatus bc_program_var_add(BcProgram* p, BcVar* var);
BcStatus bc_program_array_add(BcProgram* p, BcArray* array);
BcStatus bc_program_exec(BcProgram* p);
void bc_program_free(BcProgram* program);

#endif // BC_PROGRAM_H