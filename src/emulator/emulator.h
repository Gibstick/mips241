/**
 * Functions for actually running things in the emulator
 */
#ifndef __EMULATOR_H__
#define __EMULATOR_H__

#include <stdint.h>
#include <stdbool.h>
#include "common/defs.h"
#include "machine/machine.h"


// Load a program from a FILE into the machine at the offset.
void load_program(FILE *const file,
                  Machine *const machine,
                  uint32_t offset);

// Must call this function on startup to initialize things. 
void init_emulator(void);

EmulatorStatus step_machine(Machine *const machine);

#endif
