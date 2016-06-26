/*
 * Function for decoding an instruction.
 */
#ifndef __INSTRUCTION_H__
#define __INSTRUCTION_H__

#include <stdint.h>
#include "common/defs.h"

// Decode an instruction from a 32-bit word.
// NOTE: This function does not check for a valid instruction. 
// The caller must check for a valid opcode in the return value.
Instruction decode_instruction(uint32_t);

#endif
