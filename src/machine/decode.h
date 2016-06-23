#ifndef __INSTRUCTION_H__
#define __INSTRUCTION_H__

#include <stdint.h>
#include <stdbool.h>

#define FUNC_ADD    0x20
#define FUNC_SUB    0x22
#define FUNC_MULTU  0x19
#define FUNC_MULT   0x18
#define FUNC_DIV    0x1A
#define FUNC_DIVU   0x1B
#define FUNC_MFLO   0x10
#define FUNC_MFHI   0x12
#define FUNC_LIS    0x14
#define FUNC_SLT    0x2A
#define FUNC_SLTU   0x2B
#define OP_BEQ    0x04
#define OP_BNE    0x05
#define FUNC_JR     0x08
#define FUNC_JALR   0x09
#define OP_LW     0x23
#define OP_SW     0x2B

struct R_Type {
};

struct I_Type {
};

typedef struct Instruction {
   enum {
       TYPE_INVALID = 0,
       TYPE_R = 1,
       TYPE_I = 2
   } type;
   union {
       struct {
            uint8_t d;
            uint8_t s;
            uint8_t t;
       } r;
       struct {
            uint8_t s;
            uint8_t t;
            uint16_t imm;
       } i;
   } decoded;  
   uint8_t code; // func or opcode, depending on type
} Instruction;


// Decode an instruction from a 32-bit word.
// NOTE: This function does not check for a valid instruction. 
// The caller must check for a valid opcode in the return value.
Instruction decode_instruction(uint32_t);

#endif
