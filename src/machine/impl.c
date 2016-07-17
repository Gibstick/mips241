#include <assert.h>
#include <stdio.h>
#include <stdint.h>
#include "common/defs.h"
#include "machine/impl.h"
#include "machine/machine.h"
#include "machine/decode.h"
#include "util/util.h"

// Macros
#define R_REG(REGISTER) machine->registers[ins.decoded.r.REGISTER]
#define I_REG(REGISTER) machine->registers[ins.decoded.i.REGISTER]
#define M_REG(REGISTER) machine->registers[REGISTER]

// Macro for converting a uint32_t to an int32_t safely
#define make_signed(X) \
       (int32_t)( (X > INT32_MAX) ? X - UINT32_MAX - 1 : X )

// Constants
static const uint32_t MAPPED_INPUT_ADDR = 0xFFFF000C;
static const uint32_t MAPPED_OUTPUT_ADDR = 0xFFFF0004;

// Execute one instruction as pointed to by pc.
// Effects: machine state is modified
EmulatorStatus step_machine(Machine *const machine) {
    // Follows the fetch-decode-execute loop taught in CS 241,
    // more or less.

    if (machine->pc % 4 != 0)
        return (EmulatorStatus) {IR_UNALIGNED_INSTRUCTION_FETCH, machine->pc};

    // return value false means the machine has finished
    if (machine->pc == RETURN_ADDRESS)
        return (EmulatorStatus) {IR_DONE, machine->pc};

    // Fetch and decode
    const Instruction ins = decode_instruction(machine->mem[machine->pc / 4]);
    machine->pc += 4;

    // because I don't want to indent more
    switch (ins.type) {
        case TYPE_R:
            goto DO_RTYPE;
        case TYPE_I:
            goto DO_ITYPE;
        default:
            give_up("Internal emulator error: Invalid instruction return code. Bye.", 1, machine);
            assert(false); // squelch warnings
    }

DO_RTYPE:;
    switch (ins.code) {
        case FUNC_ADD:
        {
            R_REG(d) = R_REG(s) + R_REG(t);
            break;
        }

        case FUNC_SUB:
        {
            R_REG(d) = R_REG(s) - R_REG(t);
            break;
        }

        case FUNC_MULTU:
        {
            const uint64_t result = (int64_t)R_REG(s) * R_REG(t);
            machine->hi = result >> 32;
            machine->lo = result & 0xFFFFFFFF;
            break;
        }

        case FUNC_MULT:
        {
            const uint64_t result = (int64_t)make_signed(R_REG(s)) * make_signed(R_REG(t));
            machine->hi = result >> 32;
            machine->lo = result & 0xFFFFFFFF;
            break;
        }

        case FUNC_DIV:
        {
            machine->lo = make_signed(R_REG(s)) / make_signed(R_REG(t));
            machine->hi = make_signed(R_REG(s)) % make_signed(R_REG(t));
            break;
        }

        case FUNC_DIVU:
        {
            machine->lo = R_REG(s) / R_REG(t);
            machine->hi = R_REG(s) % R_REG(t);
            break;
        }

        case FUNC_MFLO:
        {
            R_REG(d) = machine->lo;
            break;
        }

        case FUNC_MFHI:
        {
            R_REG(d) = machine->hi;
            break;
        }

        case FUNC_LIS:
        {
            R_REG(d) = machine->mem[machine->pc / 4];
            machine->pc += 4;
            break;
        }

        case FUNC_SLT:
        {
            R_REG(d) = make_signed(R_REG(s)) < make_signed(R_REG(t));
            break;
        }

        case FUNC_SLTU:
        {
            R_REG(d) = R_REG(s) < R_REG(t);
            break;
        }

        case FUNC_JR:
        {
            machine->pc = R_REG(s);
            break;
        }

        case FUNC_JALR:
        {
            const uint32_t temp = R_REG(s);
            M_REG(31) = machine->pc;
            machine->pc = temp;
            break;
        }
    }
    goto FINISH;

DO_ITYPE:;
    const int16_t immediate = ins.decoded.i.imm;
    const int64_t byte_addr = (I_REG(s) + immediate);
    const int64_t word_addr = byte_addr / 4;

    // check for bad memory access and return accordingly
    if (ins.code == OP_LW || ins.code == OP_SW) {
        if (word_addr < 0 || word_addr >= machine->mem_size) {
            return (EmulatorStatus) {IR_OUT_OF_RANGE_MEMORY_ACCESS, byte_addr};
        }
        if (byte_addr % 4 == 0) {
            return (EmulatorStatus) {IR_UNALIGNED_MEMORY_ACCESS, byte_addr};
        }
    }

    switch (ins.code) {
        case OP_LW:
            if (byte_addr == MAPPED_INPUT_ADDR) {
                // TODO: does the reference emulator do this?
                int c = getchar();
                if (c != EOF)
                    machine->registers[ins.decoded.i.t] = c;
            } else {
                I_REG(t) = machine->mem[word_addr];
            }
            break;
        case OP_SW:
            if (byte_addr == MAPPED_OUTPUT_ADDR)
                printf("%c", (char) (I_REG(t) & 0xFF));
            else
                machine->mem[word_addr] = I_REG(t);

            break;
        case OP_BEQ:
            machine->pc += (immediate * 4 * I_REG(s) == I_REG(t));
            break;
        case OP_BNE:
            machine->pc += (immediate * 4 * I_REG(s) != I_REG(t));
    }

FINISH:
    machine->registers[0] = 0;
    return (EmulatorStatus) {IR_SUCCESS, machine->pc};
}

EmulatorStatus step_machine_loop(Machine *const machine) {
    EmulatorStatus status;
    do status = step_machine(machine);
    while (status.retcode == IR_SUCCESS);

    return status;
}
