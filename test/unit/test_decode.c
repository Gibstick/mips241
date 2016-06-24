#include "minunit.h"
#include "machine/decode.h"
#include <stdio.h>

uint32_t test_run = 0;

static const char *test_decode_add(void) {
    const uint32_t add_1_2_3 = 0x00430820; 
    Instruction ins = decode_instruction(add_1_2_3);

    mu_assert(ins.type == TYPE_R, "bad type");
    mu_assert(ins.decoded.r.d == 1, "bad $d");
    mu_assert(ins.decoded.r.s == 2, "bad $s");
    mu_assert(ins.decoded.r.t == 3, "bad $r");
    mu_assert(ins.code == FUNC_ADD, "func code");

    return NULL;
}

static const char *test_decode_sub(void) {
    const uint32_t sub_5_2_9 = 0x00492822;
    Instruction ins = decode_instruction(sub_5_2_9);

    mu_assert(ins.type == TYPE_R, "bad type");
    mu_assert(ins.decoded.r.d == 5, "bad $d");
    mu_assert(ins.decoded.r.s == 2, "bad $s");
    mu_assert(ins.decoded.r.t == 9, "bad $r");
    mu_assert(ins.code == FUNC_SUB, "bad func code");

    return NULL;
}

static const char *all_tests(void) {
    mu_run_test(test_decode_add);
    mu_run_test(test_decode_sub);
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