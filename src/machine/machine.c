#include <stdint.h>
#include <stdlib.h>
#include "util/util.h"
#include "machine/machine.h"

// assign this to any machine that needs zeroing
static const Machine zeroed_machine = { 0 };

Machine *init_machine(uint32_t max_memory_bytes) {
    give_up_unless(max_memory_bytes % 4 == 0,
                   "max_memory_bytes must be divisible by 4",
                   EXIT_FAILURE,
                   NULL
                  );

    Machine *m = malloc(sizeof(Machine));
    give_up_unless(m != NULL, "Bye bye memory", EXIT_FAILURE, m);
    *m = zeroed_machine;

    // avoiding branching because I wouldn't write C otherwise
    const uint32_t num_words =
        (max_memory_bytes == 0) * NUM_WORDS_MEMORY
        + (max_memory_bytes != 0) * (max_memory_bytes / 4);

    m->mem = malloc(sizeof(uint32_t) * num_words);
    give_up_unless(m->mem != NULL, "Can't even malloc. Bye.", EXIT_FAILURE, m);
    m->mem_size = num_words;

    return m;
}


void destroy_machine(Machine *machine) {
    if (machine != NULL) {
        free(machine->mem);
        free(machine);
    }
}


void m_print_registers(const Machine *const machine) {
    for (uint8_t i = 0; i < NUM_REGISTERS; ++i) {
        fprintf(stderr, "register %2d: 0x%08x\n", i, machine->registers[i]);
    }
}

