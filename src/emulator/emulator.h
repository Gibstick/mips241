#ifndef __EMULATOR_H__
#define __EMULATOR_H__

#include <stdint.h>
#include <stdbool.h>
#include "machine/impl.h"

// forward declarations
typedef struct Machine Machine; 

typedef struct EmulatorStatus {
    enum instruction_retcode retcode;
    uint32_t pc; 
} EmulatorStatus;

void load_program(FILE *const file, // _LUA_EXPORT
                  Machine *const machine,     // _LUA_EXPORT
                  uint32_t offset);           // _LUA_EXPORT 

EmulatorStatus step_machine(Machine *const machine);    // _LUA_EXPORT

#endif
