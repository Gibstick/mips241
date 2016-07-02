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

// Interpet one instruction and advance the machine state
EmulatorStatus step_machine(Machine *const machine);

// Interpet as many instructions as we can until 
//   we are required to stop.
EmulatorStatus step_machine_loop(Machine *const machine);

// Dump memory to the file
void dump_memory(const Machine *const machine, const char *filename);

#endif
