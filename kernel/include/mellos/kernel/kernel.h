#pragma once
#include "stdint.h"

#ifndef asmlinkage
# if defined(__i386__) && (defined(__GNUC__) || defined(__clang__))
#  define asmlinkage __attribute__((regparm(0)))
# else
#  define asmlinkage extern
# endif
#endif

extern  void kpanic(struct regs *r);
void kpanic_message(const char* msg);
_Noreturn void higher_half_main(uintptr_t multiboot_tags_addr);
