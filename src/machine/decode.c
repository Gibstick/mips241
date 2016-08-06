#include "machine/decode.h"
#include "common/defs.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

static const char *UNKNOWN_INSTRUCTION = "unknown";
static const int UNKNOWN_LENGTH = 7;

Instruction decode_instruction(uint32_t word) {
    const uint8_t s = (word >> 21) & 0x1F; //0b11111 (5)
    const uint8_t t = (word >> 16) & 0x1F;
    const uint8_t d = (word >> 11) & 0x1F;

    const uint16_t immediate_u = word & 0xFFFF;
    const int16_t immediate =
        (immediate_u > INT16_MAX) ? immediate_u - UINT16_MAX - 1 : immediate_u;

    const uint8_t opcode = (word >> 26);
    const uint8_t func = word & 0x3F; // 0b111111 (6)

    if (opcode == 0) {
        return (Instruction) {
            .type = TYPE_R,
            .decoded = {.r = { .d = d, .s = s, .t = t }},
            .code = func
        };
    } else {
        return (Instruction) {
            .type = TYPE_I,
            .decoded = {.i = { .s = s, .t = t, .imm = immediate }},
            .code = opcode
        };
    }


    // Note: invalid instructions will be caught at dispatch
}

int instruction_str(const Instruction ins, char *const outbuf, size_t size) {
    const char * mnemonic = "";
    if (size == 0) return 0;

    size_t retval = 0;

#define MNEMONIC_CASE(CODE, STR) \
    case CODE: \
        mnemonic = STR; break;

    // determine mnemonic
    // two cases are needed because some opcodes
    // have the same values as funcodes
    if (ins.type == TYPE_R) {
        switch (ins.code) {
            MNEMONIC_CASE(FUNC_ADD, "add");
            MNEMONIC_CASE(FUNC_SUB, "sub");
            MNEMONIC_CASE(FUNC_MULT, "mult");
            MNEMONIC_CASE(FUNC_MULTU, "multu");
            MNEMONIC_CASE(FUNC_DIV, "div");
            MNEMONIC_CASE(FUNC_DIVU, "divu");
            MNEMONIC_CASE(FUNC_MFHI, "mfhi");
            MNEMONIC_CASE(FUNC_MFLO, "mflo");
            MNEMONIC_CASE(FUNC_LIS, "lis");
            MNEMONIC_CASE(FUNC_SLT, "slt");
            MNEMONIC_CASE(FUNC_SLTU, "sltu");
            MNEMONIC_CASE(FUNC_JR, "jr");
            MNEMONIC_CASE(FUNC_JALR, "jalr");
            default:
               mnemonic = UNKNOWN_INSTRUCTION;
        }
    } else if (ins.type == TYPE_I) {
        switch (ins.code) {
            MNEMONIC_CASE(OP_LW, "lw");
            MNEMONIC_CASE(OP_SW, "sw");
            MNEMONIC_CASE(OP_BEQ, "beq");
            MNEMONIC_CASE(OP_BNE, "bne");
            default:
              mnemonic = UNKNOWN_INSTRUCTION;
        }
    } else {
        if (size < UNKNOWN_LENGTH + 1) return -1;
        strcpy(outbuf, UNKNOWN_INSTRUCTION);
        return UNKNOWN_LENGTH;
    }

    // determine registers/data
    if (ins.type == TYPE_R) {
        switch (ins.code) {
            case FUNC_ADD:
            case FUNC_SUB:
            case FUNC_SLT:
            case FUNC_SLTU:
              retval = snprintf(outbuf, size, "%s $%d, $%d, $%d", mnemonic,
                                ins.decoded.r.d,
                                ins.decoded.r.s,
                                ins.decoded.r.t);
              break;
            case FUNC_MULT:
            case FUNC_MULTU:
            case FUNC_DIV:
            case FUNC_DIVU:
              retval = snprintf(outbuf, size, "%s $%d, $%d", mnemonic,
                                ins.decoded.r.s,
                                ins.decoded.r.t);
              break;
            case FUNC_MFHI:
            case FUNC_MFLO:
            case FUNC_LIS:
              retval = snprintf(outbuf, size, "%s $%d", mnemonic,
                                ins.decoded.r.d);
              break;
            case FUNC_JR:
            case FUNC_JALR:
              retval = snprintf(outbuf, size, "%s $%d", mnemonic,
                                ins.decoded.r.s);
              break;
            default:
              retval = -1;
        } // invalid instruction already caught above
    } else {
        switch (ins.code) {
            case OP_LW:
            case OP_SW:
                retval = snprintf(outbuf, size, "%s $%d, %d($%d)", mnemonic,
                                  ins.decoded.i.t,
                                  ins.decoded.i.imm,
                                  ins.decoded.i.s);
                break;
            case OP_BEQ:
            case OP_BNE:
                // 0x%08x
                retval = snprintf(outbuf, size, "%s $%d, $%d, %d", mnemonic,
                                  ins.decoded.i.s,
                                  ins.decoded.i.t,
                                  ins.decoded.i.imm);
                break;
        }
    }
    if (retval >= size)
        return -1;
    else
        return retval;
}
