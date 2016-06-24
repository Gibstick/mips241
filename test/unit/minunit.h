/* file: minunit.h */
/* from http://www.jera.com/techinfo/jtns/jtn002.html */
// Note: flipped params of mu_assert


#define mu_assert(test, message) do { current_test_name = __func__; if (!(test)) return message; } while (0)
#define mu_run_test(test) do { const char *message = test(); tests_run++; \
                               if (message != NULL) return message; } while (0)
#define mu_print_failing_test() do { printf("%s\n", current_test_name); } while (0)

static int tests_run = 0;
static const char *current_test_name = "";
