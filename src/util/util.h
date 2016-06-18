#ifndef __UTIL_H__
#define __UTIL_H__
// Utility functions 

#include <stdio.h>
#include <stdbool.h>
#include <stdnoreturn.h>
#include "machine/machine.h"

#define PROGRAM_FAILURE 2 // when the MIPS program fails

// Make the program give up on life. Before that,
//   it frees memory and prints out message, then exits with status. 
static inline void give_up(const char *message, int status, Machine *machine) {
    m_print_registers(machine);
    destroy_machine(machine);
    fprintf(stderr, "%s\n", message);
    exit(status); 
}

// Make the program give up if the condition is not true.
//   Frees memory before exiting.
static inline void give_up_unless(bool condition, const char *message, int status, Machine *machine) {
    if (!condition) give_up(message, status, machine);
}
#endif
