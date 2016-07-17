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


// 16 megabytes of memory
void load_program(FILE * const infile, Machine *const machine, uint32_t offset) {


    // load the entire program into memory
    give_up_unless(infile != NULL, "Invalid file handle for load_program", EXIT_FAILURE, machine);
    give_up_unless(machine != NULL, "Null pointer for machine", EXIT_FAILURE, machine);

    uint32_t curword;
    uint32_t i = 0;
    while (fread(&curword, sizeof(curword), 1, infile) != 0) {
        if (is_little_endian())
            curword = bswap_32(curword);
        machine->mem[i + offset] = curword;
        ++i;
        give_up_unless(i < machine->mem_size, "Your program is too big", PROGRAM_FAILURE, machine);
    }
    machine->pc = offset;
    machine->registers[31] = RETURN_ADDRESS;
    fclose(infile);
}


void dump_memory(const Machine *const machine, const char *filename) {
    FILE *dumpfile = fopen(filename, "wb");

    if (dumpfile == NULL) {
        fprintf(stderr, "Unable to open file %s for memory dump.", filename);
        return;
    }

    for (size_t i = 0; i < machine->mem_size; ++i) {
        const uint32_t word = bswap_32(machine->mem[i]);
        size_t ret = fwrite(&word, sizeof(word), 1, dumpfile);

        if (ret != 1) {
            fprintf(stderr, "Memory dump to file %s failed.", filename);
            break;
        }
    }

    fclose(dumpfile);
}

void init_emulator(void) {
}
