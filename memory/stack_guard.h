#pragma once
#define STACK_CANARY_VALUE 0xDEADBEEFCAFEBABE

#include "../utils/typedefs.h"

extern uint64_t __stack_chk_guard;

void __stack_chk_fail(void);
