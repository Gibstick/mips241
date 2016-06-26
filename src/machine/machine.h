/**
 * The structure and associated functions for the Machine structure,
 * encapsulating all state for a * CS 241 MIPS machine.
 */

#ifndef __MACHINE_H__
#define __MACHINE_H__

#include <stdint.h>

#define NUM_WORDS_MEMORY 4194304 // 16 MB of RAM by default
#define NUM_REGISTERS 32         // does not include pc, ir, lo, hi 

typedef struct Machine {
    uint32_t *mem;      // array of words of memory  
    uint32_t mem_size;  // size of the above
    uint32_t registers[NUM_REGISTERS];
    uint32_t pc;        // need to divide by 4 if we want to index registers
    uint32_t hi;
    uint32_t lo;
} Machine;

// Returns a pointer to a ready-to-use struct representing 
//   the underlying MIPS machine.
//   If max_memory is nonzero, then this amount of memory
//   is given to the machine in bytes. Otherwise,
//   the default amount of memory is used. 
// Requires: max_memory_bytes is divisible by 4
Machine *init_machine(uint32_t max_memory_bytes);


// Frees all memory associated with a Machine.
// Requires: machine allocated with init_machine
// Effects: memory freed
void destroy_machine(Machine *machine);


// Prints all registers to stderr.
// Effects: output
void m_print_registers(const Machine *const machine);

#endif
