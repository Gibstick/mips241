/*
 * Function for decoding an instruction.
 */
#ifndef INSTRUCTION_H__
#define INSTRUCTION_H__

#include <stdint.h>
#include <stddef.h>
#include "common/defs.h"

// Decode an instruction from a 32-bit word.
// NOTE: This function does not check for a valid instruction.
// The caller must check for a valid opcode in the return value.
Instruction decode_instruction(uint32_t);


// Get a string representation of ins and store it in outbuf.  The size of
// the buffer is passed as size, while the number of characters (strlen) is
// returned. A negative value indicates failure.  The function checks that
// outbuf has enough space for the null terminator as well.
// 
// If the return value is nonnegative, outbuf will contain a valid null-terminated
// string.  If the return value is negative, do not assume the buffer contains
// a valid null-terminated string.
// 
// Requires: actual size of outbuf is at least as big as size
int instruction_str(const Instruction ins, char *const outbuf, size_t size);

#endif
