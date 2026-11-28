#pragma once
#include "stdint.h"
#include "cpu/idt.h"

#ifndef asmlinkage
#if defined(__i386__) && (defined(__GNUC__) || defined(__clang__))
#define asmlinkage __attribute__((regparm(0)))
#else
#define asmlinkage extern
#endif
#endif


extern void kpanic(regs_t* r);

/**
 * Prefix the message with the name of the function that the panic was called from, example:
 * "example_function: x is NULL"
 * so that it is easy to grep "example_function("
 * @param msg the message to provide in the panic screen
 */
void kpanic_message(const char* msg);
_Noreturn void higher_half_main(uintptr_t multiboot_tags_addr);