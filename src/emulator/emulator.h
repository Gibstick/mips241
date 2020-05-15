/**
 * Functions for actually running things in the emulator
 */
#ifndef EMULATOR_H__
#define EMULATOR_H__

#include <stdint.h>
#include <stdbool.h>
#include "common/defs.h"
#include "machine/machine.h"


// Load a program from a FILE into the machine at the offset.
mips241_EXPORT void load_program(FILE *const file,
                  Machine *const machine,
                  uint32_t offset);

// Must call this function on startup to initialize things.
mips241_EXPORT void init_emulator(void);

// Dump memory to the file
mips241_EXPORT void dump_memory(const Machine *const machine,
                  const char *filename);

#endif
