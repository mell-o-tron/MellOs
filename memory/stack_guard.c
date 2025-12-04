#include "mellos/kernel/kernel.h"
#include "stack_guard.h"
#include "stdint.h"

uint64_t __stack_chk_guard = STACK_CANARY_VALUE;

void __stack_chk_fail(void) {
    kpanic_message("Stack overflow detected!");
}
