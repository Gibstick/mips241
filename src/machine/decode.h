/*
 * Function for decoding an instruction.
 */
#ifndef INSTRUCTION_H__
#define INSTRUCTION_H__

#include <stdint.h>
#include "common/defs.h"

// Decode an instruction from a 32-bit word.
// NOTE: This function does not check for a valid instruction.
// The caller must check for a valid opcode in the return value.
mips241_EXPORT Instruction decode_instruction(uint32_t);

#endif
