#ifndef __IMPL_H__
#define __IMPL_H__

#include <stdint.h>
#include <stdbool.h>
#include "machine/machine.h"
#include "machine/decode.h"

#define RTYPE_TABLE_SIZE 64
#define ITYPE_TABLE_SIZE 64

enum instruction_retcode {
    IR_DONE = 0,
    IR_SUCCESS = 1,
    IR_UNALIGNED_MEMORY_ACCESS,
    IR_UNALIGNED_INSTRUCTION_FETCH,
    IR_OUT_OF_RANGE_MEMORY_ACCESS,
    IR_OUT_OF_RANGE_INSTRUCTION_FETCH,
    IR_INVALID_INSTRUCTION
};


// Must call this function on startup to initialize the dispatch table(s)!
// Boolean indicates success.
bool init_tables(void);

// Table of function pointers for r-type instructions
// indexed by 6-bit func code
extern enum instruction_retcode (*RTYPE_TABLE[RTYPE_TABLE_SIZE]) (Machine * const, const Instruction * const);
// i-type
extern enum instruction_retcode (*ITYPE_TABLE[ITYPE_TABLE_SIZE]) (Machine * const, const Instruction * const);

#endif 
