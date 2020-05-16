/*
 * Definitions for types and structures, other than the machine structure
 */
#ifndef __DEFS_H__
#define __DEFS_H__

#include <stdint.h>

/* Windows dll needs this declaration for library export */
#if (defined(_MSC_VER) || defined(__MINGW32__))
    #define mips241_EXPORT __declspec(dllexport)
#else
    #define mips241_EXPORT
#endif

#define FUNC_ADD    0x20
#define FUNC_SUB    0x22
#define FUNC_MULTU  0x19
#define FUNC_MULT   0x18
#define FUNC_DIV    0x1A
#define FUNC_DIVU   0x1B
#define FUNC_MFLO   0x12
#define FUNC_MFHI   0x10
#define FUNC_LIS    0x14
#define FUNC_SLT    0x2A
#define FUNC_SLTU   0x2B
#define OP_BEQ      0x04
#define OP_BNE      0x05
#define FUNC_JR     0x08
#define FUNC_JALR   0x09
#define OP_LW       0x23
#define OP_SW       0x2B

static const uint32_t RETURN_ADDRESS = 0x8123456c;

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
            int16_t imm;
       } i;
   } decoded;
   uint8_t code; // func or opcode, depending on type
} Instruction;

enum instruction_retcode {
    IR_DONE = 0,
    IR_SUCCESS = 1,
    IR_UNALIGNED_MEMORY_ACCESS,
    IR_UNALIGNED_INSTRUCTION_FETCH,
    IR_OUT_OF_RANGE_MEMORY_ACCESS,
    IR_OUT_OF_RANGE_INSTRUCTION_FETCH,
    IR_INVALID_INSTRUCTION,
    IR_BREAKPOINT
};

typedef struct EmulatorStatus {
    enum instruction_retcode retcode;
    uint32_t pc;
} EmulatorStatus;

#endif
