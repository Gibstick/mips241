#include "minunit.h"
#include "machine/decode.h"
#include <stdio.h>

// Helper functions for testing boilerplate
static const char *_test_decode_rtype(
    const uint32_t to_decode,
    const uint8_t d,
    const uint8_t s,
    const uint8_t t,
    const uint8_t func,
    const char *testname
) {
    current_test_name = testname;
    Instruction ins = decode_instruction(to_decode);

    mu_assert(ins.type == TYPE_R, "not r-type");
    mu_assert(ins.decoded.r.d == d, "bad $d");
    mu_assert(ins.decoded.r.s == s, "bad $s");
    mu_assert(ins.decoded.r.t == t, "bad $r");
    mu_assert(ins.code == func, "bad func code");

    return NULL;
}
// Need this wrapper so that the test name will be set properly
#define test_decode_rtype(TO_DECODE, D, S, T, FUNC) \
    _test_decode_rtype(TO_DECODE, D, S, T, FUNC, __func__)


static const char *_test_decode_itype(
    const uint32_t to_decode,
    const uint8_t s,
    const uint8_t t,
    const int16_t imm,
    const uint8_t opcode,
    const char *testname
) {
    current_test_name = testname;
    Instruction ins = decode_instruction(to_decode);

    mu_assert(ins.type == TYPE_I, "not i-type");
    mu_assert(ins.decoded.i.s == s, "bad $d");
    mu_assert(ins.decoded.i.t == t, "bad $s");
    mu_assert(ins.decoded.i.imm == imm, "bad immediate value");
    mu_assert(ins.code == opcode, "bad opcode");

    return NULL;
}
#define test_decode_itype(TO_DECODE, S, T, IMM, OPCODE) \
    _test_decode_itype(TO_DECODE, S, T, IMM, OPCODE, __func__)


static const char *test_decode_add(void) {
    return test_decode_rtype(0x00430820, 1, 2, 3, FUNC_ADD);
}

static const char *test_decode_sub(void) {
    return test_decode_rtype(0x00492822, 5, 2, 9, FUNC_SUB);
}

static const char *test_decode_mult(void) {
    return test_decode_rtype(0x00c70018, 0, 6, 7, FUNC_MULT); }

static const char *test_decode_multu(void) {
    return test_decode_rtype(0x01090019, 0, 8, 9, FUNC_MULTU);
}

static const char *test_decode_div(void) {
    return test_decode_rtype(0x014b001a, 0, 10, 11, FUNC_DIV);
}

static const char *test_decode_divu(void) {
    return test_decode_rtype(0x018d001b, 0, 12, 13, FUNC_DIVU);
}

static const char *test_decode_mfhi(void) {
    return test_decode_rtype(0x00007810, 15, 0, 0, FUNC_MFHI);
}

static const char *test_decode_mflo(void) {
    return test_decode_rtype(0x00007012, 14, 0, 0, FUNC_MFLO);
}

static const char *test_decode_lis(void) {
    return test_decode_rtype(0x00008014, 16, 0, 0, FUNC_LIS);
}

static const char *test_decode_lw(void) {
    return test_decode_itype(0x8e51ffe8, 18, 17, -24, OP_LW);
}

static const char *test_decode_sw(void) {
    return test_decode_itype(0xae930000, 20, 19, 0, OP_SW);
}

static const char *test_decode_slt(void) {
    return test_decode_rtype(0x02d7a82a, 21, 22, 23, FUNC_SLT);
}

static const char *test_decode_sltu(void) {
    return test_decode_rtype(0x03fff82b, 31, 31, 31, FUNC_SLTU);
}

static const char *test_decode_beq(void) {
    return test_decode_itype(0x10850010, 4, 5, 16, OP_BEQ);
}

static const char *test_decode_bne(void) {
    return test_decode_itype(0x1719fa0b, 24, 25, -1525, OP_BNE);
}

static const char *test_decode_jr(void) {
    return test_decode_rtype(0x03e00008, 0, 31, 0, FUNC_JR);
}

static const char *test_decode_jalr(void) {
    return test_decode_rtype(0x03400009, 0, 26, 0, FUNC_JALR);
}


static const char *all_tests(void) {
    mu_run_test(test_decode_add);
    mu_run_test(test_decode_sub);
    mu_run_test(test_decode_mult);
    mu_run_test(test_decode_multu);
    mu_run_test(test_decode_div);
    mu_run_test(test_decode_divu);
    mu_run_test(test_decode_mfhi);
    mu_run_test(test_decode_mflo);
    mu_run_test(test_decode_lis);
    mu_run_test(test_decode_lw);
    mu_run_test(test_decode_sw);
    mu_run_test(test_decode_slt);
    mu_run_test(test_decode_sltu);
    mu_run_test(test_decode_beq);
    mu_run_test(test_decode_bne);
    mu_run_test(test_decode_jr);
    mu_run_test(test_decode_jalr);
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
