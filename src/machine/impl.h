/**
 * Implemntations of all the instructions.
 */
#ifndef __IMPL_H__
#define __IMPL_H__

#include <stdint.h>
#include <stdbool.h>
#include "common/defs.h"

struct Machine;

#define RTYPE_TABLE_SIZE 64
#define ITYPE_TABLE_SIZE 64


// Must call this function on startup to initialize the dispatch table(s)!
// Boolean indicates success.
void init_tables(void);

// Table of function pointers for r-type instructions
// indexed by 6-bit func code
extern enum instruction_retcode
    (*RTYPE_TABLE[RTYPE_TABLE_SIZE]) (struct Machine * const, const Instruction * const);

// Same as above, but for i-type, indexed by opcode
extern enum instruction_retcode
    (*ITYPE_TABLE[ITYPE_TABLE_SIZE]) (struct Machine * const, const Instruction * const);

#endif 
