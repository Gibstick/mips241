#ifndef __UTIL_H__
#define __UTIL_H__
// Utility functions 

#include <stdio.h>
#include <stdbool.h>
#include "machine/machine.h"

#define PROGRAM_FAILURE 2 // when the MIPS program fails


#ifdef STDC__
    #ifdef __STDC_VERSION__
        #if defined(__STDC_VERSION) && (__STDC__VERSION__ >= 201112L) // C11
            #define noreturn __Noreturn
        #endif
    #endif
#else
    #define noreturn 
#endif

// Make the program give up on life. Before that,
//   it frees memory and prints out message, then exits with status. 
static inline noreturn void give_up(const char *message, int status, Machine *machine) {
    m_print_registers(machine);
    destroy_machine(machine);
    fprintf(stderr, "%s\n", message);
    exit(status); 
}

// Make the program give up if the condition is not true.
//   Frees memory before exiting.
static inline noreturn void give_up_unless(bool condition, const char *message, int status, Machine *machine) {
    if (!condition) give_up(message, status, machine);
}

static const uint32_t n = 1;

#define is_little_endian() (((uint8_t *)&n)[0]) \


#define bswap_32(x) \
    ((x >> 24) & UINT32_C(0xff)) |\
    ((x <<  8) & UINT32_C(0xff0000)) |\
    ((x >>  8) & UINT32_C(0xff00)) |\
    ((x << 24) & UINT32_C(0xff000000))

#endif // ifdef __UTIL_H__
