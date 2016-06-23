#include "machine/decode.h"

Instruction decode_instruction(uint32_t word) {
    const uint8_t s = (word >> 21) & 0x1F; //0b11111
    const uint8_t t = (word >> 16) & 0x1F;
    const uint8_t d = (word >> 11) & 0x1F;

    const uint16_t immediate_u = word & 0xFFFF;
    const int16_t immediate =
        (immediate_u > INT16_MAX) ? UINT16_MAX - immediate_u : immediate_u;

    const uint8_t opcode = (word >> 26);
    const uint8_t func = word & 0x1F; // l

    if (opcode == 0) {
        return (Instruction) {
            .type = TYPE_R,
            .decoded = {.r = { .d = d, .s = s, .t = t }},
            .code = func 
        };
    } else {
        return (Instruction) {
            .type = TYPE_I,
            .decoded = {.i = { .s = s, .t = t }},
            .code = opcode
        };
    }
    // Note: invalid instructions will be caught at dispatch 
}