#include "assert.h"

#include "stdio.h"
#include "unistd.h"

#ifdef __MELLOS_KERNEL_ASSERT__
#error "Userspace asserts are being used with the kernel assert header"
#endif
void __attribute__((weak)) __assert_fail(const char* expr, const char* file, unsigned int line) { // NOLINT(*-reserved-identifier)
    fprintf(stderr, "Assertion failed: %s, file %s, line %d\n", expr, file, line);
    syscall_exit(1);
}

void __attribute__((weak)) __assert_fail_msg(const char* expr, const char* message, const char* file, unsigned int line) { // NOLINT(*-reserved-identifier)
    fprintf(stderr, "Message: %s", message);
    fprintf(stderr, "Assertion failed: %s, file %s, line %d\n", expr, file, line);
    syscall_exit(1);
}