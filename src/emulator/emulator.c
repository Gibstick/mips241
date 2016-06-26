#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>
#include "emulator/emulator.h"
#include "machine/machine.h"
#include "machine/impl.h"
#include "machine/decode.h"
#include "util/util.h"
#include "common/defs.h"

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
    
    // Execute
    enum instruction_retcode ret;
    switch (ins.type) {
        case TYPE_R:
            ret = RTYPE_TABLE[ins.code](machine, &ins);
            break;
        case TYPE_I:
            ret = ITYPE_TABLE[ins.code](machine, &ins);
            break;
        default:
            give_up("Internal emulator error: Invalid instruction return code. Bye.", 1, machine);
            assert(false); // squelch warnings
    }
    machine->registers[0] = 0; // the instructions don't check for zero
    
    return (EmulatorStatus) {ret, machine->pc};
}

void init_emulator(void) {
    init_tables();    
}
