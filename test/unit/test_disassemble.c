#include "minunit.h"
#include "machine/decode.h"
#include "common/defs.h"
#include <stdio.h>
#include <string.h>

#define BUF_SIZE 100
#define FAIL_MESSAGE_SIZE 1000


static char message[FAIL_MESSAGE_SIZE];

const char *_test_disassemble(uint32_t ins, const char *expected, const char *testname) {
    current_test_name = testname;
    
    char buf[BUF_SIZE];

    int retval = instruction_str(decode_instruction(ins), buf, BUF_SIZE); 
    if (retval < 0) mu_assert(0, "Error!");

    snprintf(message, FAIL_MESSAGE_SIZE, "Expected %s but received %s\n", expected, buf);
    mu_assert(strcmp(buf, expected) == 0, message);

    return NULL;
}
#define test_disassemble(INS, EXPECTED) _test_disassemble(INS, EXPECTED, __func__)


static const char *test_add(void) {
    return test_disassemble(0x00430820, "add $1, $2, $3");
}

static const char *test_sub(void) {
    return test_disassemble(0x00492822, "sub $5, $2, $9");
}

static const char *test_mult(void) {
    return test_disassemble(0x00c70018, "mult $6, $7");
}

static const char *test_multu(void) {
    return test_disassemble(0x01090019, "multu $8, $9");
}

static const char *test_div(void) {
    return test_disassemble(0x014b001a, "div $10, $11");
}

static const char *test_divu(void) {
    return test_disassemble(0x018d001b, "divu $12, $13");
}

static const char *test_mfhi(void) {
    return test_disassemble(0x00007810, "mfhi $15");
}

static const char *test_mflo(void) {
    return test_disassemble(0x00007012, "mflo $14");
}

static const char *test_lis(void) {
    return test_disassemble(0x00008014, "lis $16");
}

static const char *test_lw(void) {
    //return test_disassemble(0x8e51ffe8, 18, 17, -24, OP_LW);
    return test_disassemble(0x8e51ffe8, "lw $17, -24($18)");
}

static const char *test_sw(void) {
    return test_disassemble(0xae930000, "sw $19, 0($20)");
}

static const char *test_slt(void) {
    return test_disassemble(0x02d7a82a, "slt $21, $22, $23");
}

static const char *test_sltu(void) {
    return test_disassemble(0x03fff82b, "sltu $31, $31, $31");
}

static const char *test_beq(void) {
    return test_disassemble(0x10850010, "beq $4, $5, 16");
}

static const char *test_bne(void) {
    return test_disassemble(0x1719fa0b, "bne $24, $25, -1525");
}

static const char *test_jr(void) {
    return test_disassemble(0x03e00008, "jr $31");
}

static const char *test_jalr(void) {
    return test_disassemble(0x03400009, "jalr $26");
}

static const char *all_tests(void) {
    mu_run_test(test_add);
    mu_run_test(test_sub);
    mu_run_test(test_mult);
    mu_run_test(test_div);
    mu_run_test(test_multu);
    mu_run_test(test_divu);
    mu_run_test(test_mflo);
    mu_run_test(test_mfhi);
    mu_run_test(test_lis);
    mu_run_test(test_lw);
    mu_run_test(test_sw);
    mu_run_test(test_slt);
    mu_run_test(test_sltu);
    mu_run_test(test_beq);
    mu_run_test(test_bne);
    mu_run_test(test_jr);
    mu_run_test(test_jalr);
    // Note: all tests must run here!
    return NULL;
}

int main(void) {
    const char *result = all_tests();

    if (result != NULL) {
        printf("Test failed: ");
        mu_print_failing_test();
        printf("%s\n", result);
    } else {
        printf("Tests passed!");
    }

    printf("Tests run: %d\n", tests_run);

    return result != NULL;
}
