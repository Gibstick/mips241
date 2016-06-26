#include <stdio.h>
#include "machine/impl.h"
#include "machine/machine.h"
#include "emulator/emulator.h"

static void print_status(FILE *out, const EmulatorStatus * const status) {
    // TODO
    static const char * status_strings[] = {
        [IR_DONE] = "Program completed successfully.",
        [IR_SUCCESS] = "Program execution paused.",
        [IR_UNALIGNED_MEMORY_ACCESS] = "Program attempted to read/write an unaligned address.",
        [IR_UNALIGNED_INSTRUCTION_FETCH] = "Program counter contains an unaligned address.",
        [IR_OUT_OF_RANGE_MEMORY_ACCESS] = "Program attempted to read/write memory that was out of bounds.",
        [IR_OUT_OF_RANGE_INSTRUCTION_FETCH] = "Program counter contains an  out-of-bounds address.",
        [IR_INVALID_INSTRUCTION] = "An invalid instruction was encountered."
    };

    fprintf(out, "%s\n", status_strings[status->retcode]);
}


int main(void) {
    init_tables();
    Machine *m = init_machine(0);

    load_program(stdin, m, 0);

    EmulatorStatus status;
    do status = step_machine(m);
    while (status.retcode == IR_SUCCESS);

    m_print_registers(m);
    print_status(stderr, &status);

    destroy_machine(m);
}
