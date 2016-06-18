#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "util/util.h"
#include "machine/machine.h"
#include "emulator/opcodes.h"
#include "emulator/emulator.h"

static const uint32_t STDOUT_ADDRESS = 0xFFFF000C;
static const uint32_t STDIN_ADDRESS = 0xFFFF0008;
static const uint32_t RETURN_ADDRESS = 0x8123456c;

static bool is_little_endian(void) {
    static const uint32_t n = 1;
    return ((uint8_t *)&n)[0];
}

// 16 megabytes of memory
void load_program(FILE * const infile, Machine *const machine, uint32_t offset) {


    // load the entire program into memory
    give_up_unless(infile != NULL, "Invalid file handle for load_program", EXIT_FAILURE, machine);
    give_up_unless(machine != NULL, "Null pointer for machine", EXIT_FAILURE, machine);

    uint32_t curword;
    uint32_t i = 0;
    while(fread(&curword, sizeof(curword), 1, infile) != 0) {
        if (is_little_endian()) {
            curword = ((curword>>24)&0xff) |
                      ((curword<<8)&0xff0000) |
                      ((curword>>8)&0xff00) |
                      ((curword<<24)&0xff000000);
        }
        machine->mem[i + offset] = curword;
        ++i;
        give_up_unless(i < machine->mem_size, "Your program is too big", PROGRAM_FAILURE, machine);
    }
    machine->pc = offset;
    machine->registers[31] = RETURN_ADDRESS;
    fclose(infile);
}


EmulatorStatus step_machine(Machine *const machine) {
    // Follows the fetch-decode-execute loop taught in CS 241,
    // more or less.

    if (machine->pc % 4 != 0)
        return (EmulatorStatus) {ER_UNALIGNED_INSTRUCTION_FETCH, machine->pc};

    // return value false means the machine has finished 
    if (machine->pc == RETURN_ADDRESS)
        return (EmulatorStatus) {ER_SUCCESS, machine->pc};

    const uint32_t cur_instruction = machine->mem[machine->pc / 4];
    machine->pc += 4;

    // alias to save typing
    uint32_t *const regs = machine->registers;
    // have mercy
    #define $(x) regs[x]

    const uint8_t s = (cur_instruction >> 21) & 0x1F;
    const uint8_t t = (cur_instruction >> 16) & 0x1F;
    const uint8_t d = (cur_instruction >> 11) & 0x1F;
    const int16_t immediate = cur_instruction & 0xFFFF; // FIXME: undefined behaviour 

    const uint8_t r_opcode = cur_instruction & 0x3F; // 0b111111
    const uint8_t i_opcode = (cur_instruction >> 26);

    // for signed operations that matter
    const int32_t lhs = $(s) > 0x7FFFFFF ? ~$(s) + 1 : $(s);
    const int32_t rhs = $(t) > 0x7FFFFFF ? ~$(t) + 1 : $(t);


    if (i_opcode == 0) {
        switch (r_opcode) {
        case ADD:
            $(d) = $(s) + $(t);
            break;
        case SUB:
            $(d) = $(s) - $(t);
            break;
        case MULTU:
        {
            // need to sign extend
            const uint64_t result = lhs * rhs;
            machine->hi = result >> 32;
            machine->lo = result & 0xFFFFFFFF;
            break;
        }
        case MULT:
        {
            const uint64_t result = $(s) * $(t);
            machine->hi = result >> 32;
            machine->lo = result & 0xFFFFFFFF;
            break;
        }
        case DIV:
        {
            machine->lo = $(s) / $(t);
            machine->hi = $(s) % $(t);
            break;
        }
        case DIVU:
        {
            machine->lo = lhs / rhs;
            machine->hi = lhs % rhs;
            break;
        }
        case MFLO:
            $(d) = machine->lo;
            break;
        case MFHI:
            $(d) = machine->hi;
            break;
        case LIS:
            $(d) = machine->mem[machine->pc / 4];
            machine->pc += 4;
            break;
        case SLT:
            $(d) = lhs < rhs;
            break;
        case SLTU:
            $(d) = $(s) < $(t);
            break;
        case JR:
            machine->pc = $(s);
            break;
        case JALR:
        {
            const uint32_t temp = $(s);
            $(31) = machine->pc;
            machine->pc = temp;
            break;
        }
        default:
            goto invalid_instruction;
        }
    } else {
        const int64_t byte_addr = ($(s) + immediate);
        const int64_t word_addr = byte_addr / 4;

        // check for bad memory access and return accordingly
        if (i_opcode == LW || i_opcode == SW) {
            if (word_addr < 0 || word_addr >= machine->mem_size) {
                return (EmulatorStatus) {ER_OUT_OF_RANGE_MEMORY_ACCESS, byte_addr};
            }
            if (byte_addr % 4 == 0) {
                return (EmulatorStatus) {ER_UNALIGNED_MEMORY_ACCESS, byte_addr};
            }
        }

        switch (i_opcode) {
        case LW:
            $(t) = machine->mem[word_addr];
            // TODO: input
            break;
        case SW:
            machine->mem[word_addr] = $(t);
            // TODO: output
            break;
        case BEQ:
            machine->pc += (immediate * 4 * $(s) == $(t));
            break;
        case BNE:
            machine->pc += (immediate * 4 * $(s) != $(t));
        default:
            goto invalid_instruction;
        }
    }

    // NOTE: ER_SUCCESS is 1 and ER_DONE is 0
    return (EmulatorStatus) {!(machine->pc == RETURN_ADDRESS), machine->pc};

invalid_instruction:
    return (EmulatorStatus) {ER_INVALID_INSTRUCTION, machine->pc};
}


int main(void) {
    Machine *m = init_machine(0);

    load_program(stdin, m, 0);

    while (step_machine(m).retcode == ER_SUCCESS);

    m_print_registers(m);
    destroy_machine(m);
}
