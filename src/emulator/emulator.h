#include <stdint.h>
#include <stdbool.h>

typedef struct Machine Machine; 

enum emulator_retcode {
    ER_DONE = 0,
    ER_SUCCESS = 1,
    ER_UNALIGNED_MEMORY_ACCESS,
    ER_UNALIGNED_INSTRUCTION_FETCH,
    ER_OUT_OF_RANGE_MEMORY_ACCESS,
    ER_OUT_OF_RANGE_INSTRUCTION_FETCH,
    ER_INVALID_INSTRUCTION
};

typedef struct EmulatorStatus {
    enum emulator_retcode retcode;
    uint32_t last_pc;
} EmulatorStatus;

void load_program(FILE *const file, // _LUA_EXPORT
                  Machine *const machine,     // _LUA_EXPORT
                  uint32_t offset);           // _LUA_EXPORT 

EmulatorStatus step_machine(Machine *const machine);    // _LUA_EXPORT


