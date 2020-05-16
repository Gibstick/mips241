/**
 * Implemntations of all the instructions.
 */
#ifndef IMPL_H__
#define IMPL_H__

#include <stdint.h>
#include <stdbool.h>
#include "common/defs.h"

struct Machine;

// Interpet one instruction and advance the machine state
mips241_EXPORT EmulatorStatus step_machine(struct Machine *const machine);

// Interpet as many instructions as we can until
//   we are required to stop.
mips241_EXPORT EmulatorStatus step_machine_loop(struct Machine *const machine);

#endif
