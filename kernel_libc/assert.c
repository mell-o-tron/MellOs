#include "assert.h"
#include "kernel_stdio.h"

void __kassert_fail(const char* expr, const char* file, unsigned int line) { // NOLINT(*-reserved-identifier)
    kprintf("Assertion failed: %s\nlocation: %s:%d\n", expr, file, line);
	asm("hlt");
}

void __kassert_fail_msg(const char* expr, const char* message, const char* file, unsigned int line) { // NOLINT(*-reserved-identifier)
    kprintf("Message: %s\n", message);
    kprintf("Assertion failed: %s\nlocation: %s:%d\n", expr, file, line);
	asm("hlt");
}